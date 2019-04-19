#include "libnewrelic.h"
#include "global.h"

#include "nr_agent.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_sleep.h"
#include "util_strings.h"

#include <stdlib.h>

bool newrelic_log_configured = false;

bool newrelic_configure_log(const char* filename, newrelic_loglevel_t level) {
  /* Map newrelic_loglevel_t ENUM to char* */
  static const char* level_array[]
      = {"error", "warning", "info", "verbosedebug"};
  static const size_t max_level = sizeof(level_array) / sizeof(const char*);

  if (NULL == filename) {
    nrl_error(NRL_API, "filename cannot be NULL");
    return false;
  }

  if (((size_t)level) >= max_level) {
    nrl_error(NRL_API,
              "level %zu is out of range; must be between 0 and %zu, inclusive",
              (size_t)level, max_level - 1);
    return false;
  }

  if (NR_SUCCESS != nrl_set_log_file(filename)) {
    nrl_error(NRL_API, "could not set the log file to %s", filename);
    return false;
  }

  if (NR_SUCCESS != nrl_set_log_level(level_array[level])) {
    nrl_error(NRL_API, "could not set the log level to %s", level_array[level]);
    return false;
  }

  newrelic_log_configured = true;
  return true;
}

bool newrelic_init(const char* daemon_socket, int time_limit_ms) {
  if (NULL != nr_agent_applist) {
    nrl_error(NRL_API, "newrelic_init() cannot be invoked more than once");
    return false;
  }

  return newrelic_do_init(daemon_socket, time_limit_ms);
}

bool newrelic_do_init(const char* daemon_socket, int time_limit_ms) {
  const char* path;

  if (!newrelic_log_configured) {
    // No log configuration; let's set some reasonable defaults and hope for
    // the best.
    if (!newrelic_configure_log("stderr", NEWRELIC_LOG_INFO)) {
      return false;
    }
  }

  path = daemon_socket ? daemon_socket : "/tmp/.newrelic.sock";

  if (NR_SUCCESS != nr_agent_initialize_daemon_connection_parameters(path, 0)) {
    nrl_error(NRL_API, "failed to initialise daemon connection to %s", path);
    return false;
  }

  if (!nr_agent_try_daemon_connect(time_limit_ms ? time_limit_ms : 10)) {
    nrl_error(NRL_API,
              "failed to connect to the daemon using a timeout of %d ms at the "
              "path %s",
              time_limit_ms, path);
    return false;
  }

  nr_agent_applist = nr_applist_create();

  atexit(newrelic_shutdown);

  nrl_info(NRL_INSTRUMENT, "newrelic initialized");
  return true;
}

bool newrelic_ensure_init(void) {
  if (NULL != nr_agent_applist) {
    return true;
  }

  // Attempt to init with default parameters.
  return newrelic_do_init(NULL, 0);
}

void newrelic_shutdown(void) {
  nr_agent_close_daemon_connection();
  nr_applist_destroy(&nr_agent_applist);
  nrl_close_log_file();
  newrelic_log_configured = false;
}

void newrelic_add_api_supportability_metric(nrtxn_t* txn, const char* name) {
  char* metric;
  if (NULL == name || NULL == txn) {
    return;
  }

  metric = nr_formatf("Supportability/C/api/%s", name);

  nrm_force_add(txn->unscoped_metrics, metric, 0);

  nr_free(metric);
}
