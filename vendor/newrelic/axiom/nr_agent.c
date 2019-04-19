#include "nr_axiom.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>

#include "nr_agent.h"
#include "util_errno.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_sleep.h"
#include "util_strings.h"
#include "util_syscalls.h"

typedef enum _nr_socket_type_t {
  NR_LISTEN_TYPE_TCP,
  NR_LISTEN_TYPE_UNIX
} nr_socket_type_t;

nrapplist_t* nr_agent_applist = 0;

static nrthread_mutex_t nr_agent_daemon_mutex = NRTHREAD_MUTEX_INITIALIZER;

static int nr_agent_daemon_fd = -1;

static struct sockaddr_in nr_agent_daemon_inaddr;
static struct sockaddr_un nr_agent_daemon_unaddr;
static struct sockaddr* nr_agent_daemon_sa = 0;
static socklen_t nr_agent_daemon_sl = 0;

static nr_socket_type_t nr_agent_desired_type;
static int nr_agent_desired_external = 0;
static char nr_agent_desired_uds[sizeof(nr_agent_daemon_unaddr.sun_path)];
static const int nr_agent_desired_uds_max
    = sizeof(nr_agent_daemon_unaddr.sun_path) - 1;

static char nr_agent_connect_method_msg[512];

#define NR_AGENT_CANT_CONNECT_WARNING_BACKOFF_SECONDS 20
static time_t nr_agent_last_cant_connect_warning = 0;

typedef enum _nr_agent_connection_state_t {
  NR_AGENT_CONNECTION_STATE_START,
  NR_AGENT_CONNECTION_STATE_IN_PROGRESS,
  NR_AGENT_CONNECTION_STATE_CONNECTED,
} nr_agent_connection_state_t;

nr_agent_connection_state_t nr_agent_connection_state
    = NR_AGENT_CONNECTION_STATE_START;

nr_status_t nr_agent_initialize_daemon_connection_parameters(
    const char* listen_path,
    int external_port) {
  /*
   * Check parameters
   */
  if (0 == external_port) {
    int len;

    if (0 == listen_path) {
      nrl_error(
          NRL_DAEMON,
          "invalid daemon connection parameters: zero port and listen path");
      return NR_FAILURE;
    }

#if NR_SYSTEM_LINUX
    /* '@' prefix specifies a Linux abstract domain socket. */
    if ('@' == listen_path[0]) {
      if (0 == listen_path[1]) {
        nrl_error(NRL_DAEMON,
                  "invalid daemon abstract domain socket: name is missing");
        return NR_FAILURE;
      }
    } else if ('/' != listen_path[0]) {
      nrl_error(NRL_DAEMON,
                "invalid daemon UNIX-domain socket: path must be absolute");
      return NR_FAILURE;
    }
#else
    if ('/' != listen_path[0]) {
      nrl_error(NRL_DAEMON,
                "invalid daemon UNIX-domain socket: path must be absolute");
      return NR_FAILURE;
    }
#endif

    len = nr_strlen(listen_path);
    if (len > nr_agent_desired_uds_max) {
      nrl_error(NRL_DAEMON, "invalid daemon UNIX-domain socket: too long");
      return NR_FAILURE;
    }
  }

  nrt_mutex_lock(&nr_agent_daemon_mutex);

  if (0 == external_port) {
    /*
     * Unix Domain Socket (see unix(7))
     */
    nr_agent_desired_type = NR_LISTEN_TYPE_UNIX;
    nr_strlcpy(nr_agent_desired_uds, listen_path, nr_agent_desired_uds_max);

    nr_agent_daemon_sa = (struct sockaddr*)&nr_agent_daemon_unaddr;
    nr_agent_daemon_sl
        = offsetof(struct sockaddr_un, sun_path) + nr_strlen(listen_path) + 1;
    nr_memset(nr_agent_daemon_sa, 0, sizeof(nr_agent_daemon_sa));

    nr_agent_daemon_unaddr.sun_family = AF_UNIX;
    nr_strlcpy(nr_agent_daemon_unaddr.sun_path, listen_path,
               sizeof(nr_agent_daemon_unaddr.sun_path));
    if ('@' == nr_agent_daemon_unaddr.sun_path[0]) {
      /* A leading zero specifies an abstract socket to the kernel. */
      nr_agent_daemon_unaddr.sun_path[0] = '\0';

      /* Exclude the trailing zero to match the behavior of Go. */
      nr_agent_daemon_sl--;
    }

    nr_agent_connect_method_msg[0] = '\0';
    snprintf(nr_agent_connect_method_msg, sizeof(nr_agent_connect_method_msg),
             "uds=%s", listen_path);
  } else {
    /*
     * Use a TCP connection.
     */
    nr_agent_desired_type = NR_LISTEN_TYPE_TCP;
    nr_agent_desired_external = external_port;

    nr_agent_daemon_sa = (struct sockaddr*)&nr_agent_daemon_inaddr;
    nr_agent_daemon_sl = sizeof(nr_agent_daemon_inaddr);
    nr_memset(nr_agent_daemon_sa, 0, (int)nr_agent_daemon_sl);

    nr_agent_daemon_inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    nr_agent_daemon_inaddr.sin_port = htons((uint16_t)external_port);
    nr_agent_daemon_inaddr.sin_family = AF_INET;

    nr_agent_connect_method_msg[0] = '\0';
    snprintf(nr_agent_connect_method_msg, sizeof(nr_agent_connect_method_msg),
             "port=%d", external_port);
  }

  nrt_mutex_unlock(&nr_agent_daemon_mutex);

  return NR_SUCCESS;
}

static int nr_agent_create_socket(nr_socket_type_t listen_type) {
  int fd;
  int fl;
  int err;

  if (NR_LISTEN_TYPE_TCP == listen_type) {
    int on = 1;

    fd = nr_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    nr_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
  } else {
    fd = nr_socket(PF_UNIX, SOCK_STREAM, 0);
  }

  if (-1 == fd) {
    err = errno;
    nrl_warning(NRL_DAEMON, "daemon socket() returned %.16s", nr_errno(err));
    return -1;
  }

  fl = nr_fcntl(fd, F_GETFL, 0);
  if (-1 == fl) {
    err = errno;
    nrl_warning(NRL_DAEMON, "daemon fcntl(GET) returned %.16s", nr_errno(err));
    nr_close(fd);
    return -1;
  }

  fl |= O_NONBLOCK;
  if (0 != nr_fcntl(fd, F_SETFL, fl)) {
    err = errno;
    nrl_warning(NRL_DAEMON, "daemon fcntl(SET) returned %.16s", nr_errno(err));
    nr_close(fd);
    return -1;
  }

  return fd;
}

static void nr_agent_warn_connect_failure(int connect_fd,
                                          int connect_rv,
                                          int connect_err) {
  time_t now = time(0);

  if ((now - nr_agent_last_cant_connect_warning)
      < NR_AGENT_CANT_CONNECT_WARNING_BACKOFF_SECONDS) {
    return;
  }

  nr_agent_last_cant_connect_warning = now;

  nrl_warning(
      NRL_DAEMON | NRL_IPC,
      "daemon connect(fd=%d %.256s) returned %d errno=%.16s. "
      "Failed to connect to the newrelic-daemon. Please make sure that there "
      "is a properly configured newrelic-daemon running. "
      "For additional assistance, please see: "
      "https://newrelic.com/docs/php/newrelic-daemon-startup-modes",
      connect_fd, nr_agent_connect_method_msg, connect_rv,
      nr_errno(connect_err));
}

static int nr_get_daemon_fd_internal(int log_warning_on_connect_failure) {
  int err;
  int fl;
  nr_agent_connection_state_t state_before_connect;

  if (NR_AGENT_CONNECTION_STATE_CONNECTED == nr_agent_connection_state) {
    return nr_agent_daemon_fd;
  }

  if (-1 == nr_agent_daemon_fd) {
    nr_agent_daemon_fd = nr_agent_create_socket(nr_agent_desired_type);
    if (-1 == nr_agent_daemon_fd) {
      return -1;
    }
  }

  state_before_connect = nr_agent_connection_state;

  do {
    fl = nr_connect(nr_agent_daemon_fd, nr_agent_daemon_sa, nr_agent_daemon_sl);
    err = errno;
  } while ((-1 == fl) && (EINTR == err));

  if (0 == fl) {
    nrl_verbosedebug(NRL_DAEMON | NRL_IPC,
                     "daemon connect(fd=%d %.256s) succeeded",
                     nr_agent_daemon_fd, nr_agent_connect_method_msg);
  } else {
    nrl_verbosedebug(NRL_DAEMON | NRL_IPC,
                     "daemon connect(fd=%d %.256s) returned %d errno=%.16s",
                     nr_agent_daemon_fd, nr_agent_connect_method_msg, fl,
                     nr_errno(err));
  }

  if ((0 == fl) || (EISCONN == err)) {
    /*
     * Since the file descriptor is non-blocking, the connect call may return
     * EINPROGRESS.  If this happens, we need to determine when the connection
     * has completed.  We do this by, repeating the connect call.  Once the
     * connection succeeded, EISCONN will be returned.  This also has the
     * advantage that we can treat first attempt connects the same as
     * in-progress connects.
     */
    nr_agent_connection_state = NR_AGENT_CONNECTION_STATE_CONNECTED;
    return nr_agent_daemon_fd;
  }

  if ((EALREADY == err) || (EINPROGRESS == err)) {
    /*
     * The connection is in progress.
     * This is not unexpected the first time this function is called.
     * However, if this is not the first time, a log warning message
     * should be generated.
     */
    nr_agent_connection_state = NR_AGENT_CONNECTION_STATE_IN_PROGRESS;
    if (log_warning_on_connect_failure
        && (NR_AGENT_CONNECTION_STATE_IN_PROGRESS == state_before_connect)) {
      nr_agent_warn_connect_failure(nr_agent_daemon_fd, fl, err);
    }
    return -1;
  }

  /*
   * The connect call failed for an unknown reason.
   */
  if (log_warning_on_connect_failure) {
    nr_agent_warn_connect_failure(nr_agent_daemon_fd, fl, err);
  }
  nr_close(nr_agent_daemon_fd);
  nr_agent_daemon_fd = -1;
  nr_agent_connection_state = NR_AGENT_CONNECTION_STATE_START;
  return -1;
}

int nr_get_daemon_fd(void) {
  int fd;

  nrt_mutex_lock(&nr_agent_daemon_mutex);

  fd = nr_get_daemon_fd_internal(1);

  nrt_mutex_unlock(&nr_agent_daemon_mutex);

  return fd;
}

int nr_agent_try_daemon_connect(int time_limit_ms) {
  int fd;
  int did_connect = 0;

  nrt_mutex_lock(&nr_agent_daemon_mutex);

  fd = nr_get_daemon_fd_internal(0);
  if (-1 != fd) {
    did_connect = 1;
  } else if (NR_AGENT_CONNECTION_STATE_IN_PROGRESS
             == nr_agent_connection_state) {
    nr_msleep(time_limit_ms);
    fd = nr_get_daemon_fd_internal(0);
    if (-1 != fd) {
      did_connect = 1;
    }
  }

  nrt_mutex_unlock(&nr_agent_daemon_mutex);

  return did_connect;
}

void nr_set_daemon_fd(int fd) {
  nrt_mutex_lock(&nr_agent_daemon_mutex);

  if (-1 != nr_agent_daemon_fd) {
    nrl_debug(NRL_DAEMON, "closed daemon connection fd=%d", nr_agent_daemon_fd);
    nr_close(nr_agent_daemon_fd);
    nr_agent_daemon_fd = -1;
  }

  nr_agent_daemon_fd = fd;
  nr_agent_last_cant_connect_warning = 0;
  nr_agent_connection_state = NR_AGENT_CONNECTION_STATE_START;

  if (-1 != nr_agent_daemon_fd) {
    nr_agent_connection_state = NR_AGENT_CONNECTION_STATE_CONNECTED;
  }

  nrt_mutex_unlock(&nr_agent_daemon_mutex);
}

void nr_agent_close_daemon_connection(void) {
  nr_set_daemon_fd(-1);
}

nr_status_t nr_agent_lock_daemon_mutex(void) {
  return nrt_mutex_lock(&nr_agent_daemon_mutex);
}

nr_status_t nr_agent_unlock_daemon_mutex(void) {
  return nrt_mutex_unlock(&nr_agent_daemon_mutex);
}
