/*
 * This file contains functions to manage the agent's connection to the daemon.
 */
#ifndef NR_AGENT_HDR
#define NR_AGENT_HDR

#include "nr_axiom.h"
#include "nr_app.h"

/*
 * Purpose : This is the agent's global applist.
 *
 * Note    : There is no locking around this application list.  Therefore
 *           it should be created before and destroyed after multiple threads
 *           have access to it.
 */
extern nrapplist_t* nr_agent_applist;

extern nr_status_t nr_agent_initialize_daemon_connection_parameters(
    const char* listen_path,
    int external_port);

/*
 * Purpose : Returns the file descriptor used to communicate with the daemon.
 *           If the daemon failed to initialize or the connection has been lost
 *           or closed, will return -1.
 *
 * Returns : The daemon file descriptor or -1.
 *
 * Notes   : After this function is called, this process must call
 *           nr_agent_close_daemon_connection before forking.  This must
 *           be done even if nr_get_daemon_fd does not return a valid
 *           fd, as the connection may be in progress.
 *
 *           This approach is unsafe for threaded processes:
 *           Any thread which gets a file descriptor using this function
 *           can not guarantee that another thread does not close the fd.
 */
extern int nr_get_daemon_fd(void);

/*
 * Purpose : Set the connection to use for daemon communication.
 *
 * Params  : 1. An established connection to a daemon process.
 */
extern void nr_set_daemon_fd(int fd);

/*
 * Purpose : Close the connection between an agent process and the daemon.
 *
 * Params  : None.
 *
 * Returns : Nothing.
 *
 * Notes   : Only called from within a agent process. This is called when an
 *           error has been detected by the agent when trying to communicate
 *           with the daemon.
 */
extern void nr_agent_close_daemon_connection(void);

/*
 * Purpose : Determine if a connection to the daemon is possible by creating
 *           one.  This differs from nr_get_daemon_fd in two ways: If the
 *           connection attempt fails, no warning messages will be printed,
 *           and if the connection attempt fails then it will be retried
 *           after a time_limit_ms delay.
 *
 * Returns : 1 if a connection to the daemon succeeded, and 0 otherwise.
 */
extern int nr_agent_try_daemon_connect(int time_limit_ms);

/*
 * Purpose : Lock or unlock access to the daemon from within an agent process.
 *           This is used to ensure that only one thread within an agent can
 *           ever be communicating with the daemon at 1 time, in order to
 *           prevent data interleaving and trying to multiplex commands and
 *           their replies.
 *
 * Params  : None.
 *
 * Returns : NR_SUCCESS or NR_FAILURE.
 *
 * Notes   : Only used within child processes.
 */
extern nr_status_t nr_agent_lock_daemon_mutex(void);
extern nr_status_t nr_agent_unlock_daemon_mutex(void);

#endif /* NR_AGENT_HDR */
