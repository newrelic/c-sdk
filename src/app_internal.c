/*
 * The functions in this file are not in app.c so they can be mocked in the
 * unit tests.
 */

#include "libnewrelic.h"
#include "app.h"

#include "nr_agent.h"
#include "util_logging.h"
#include "util_sleep.h"

nr_status_t newrelic_connect_app(newrelic_app_t* app,
                                 unsigned short timeout_ms) {
  nrapp_t* nrapp;
  nrtime_t timeout_time = timeout_ms * NR_TIME_DIVISOR_MS;

  if (NULL == app) {
    nrl_error(NRL_INSTRUMENT, "invalid application");
    return NR_FAILURE;
  }

  if (NULL == app->app_info) {
    nrl_error(NRL_INSTRUMENT, "application with invalid information");
    return NR_FAILURE;
  }

  nrapp = nr_agent_find_or_add_app(nr_agent_applist, app->app_info, NULL,
                                   timeout_time);
  if (NULL == nrapp) {
    nrl_error(NRL_INSTRUMENT, "application was unable to connect");
    return NR_FAILURE;
  }

  app->app = nrapp;
  nrt_mutex_unlock(&app->app->app_lock);
  nrl_info(NRL_INSTRUMENT, "application %s connected",
           NRSAFESTR(app->app_info->appname));
  nrt_mutex_init(&app->lock, 0);

  return NR_SUCCESS;
}
