#define _GNU_SOURCE

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libnewrelic.h"
#include "common.h"

static newrelic_app_t* test_app_create_app(const char* license,
                                           const char* host,
                                           const char* app_name);
/*
 * A small app that sends a transaction when the user hints enter.
 * Useful for testing daemon behavior that requires precise timing.
 */
int main(void) {
  newrelic_app_t* app = 0;
  newrelic_txn_t* txn = 0;

  char* license = get_license_key();
  if (!license) {
    return EXIT_FAILURE;
  }

  char* app_name = get_app_name();
  if (!app_name) {
    return EXIT_FAILURE;
  }

  // if host isn't defined we just won't assign
  // it, and the agent will use the default
  char* host = getenv("NEW_RELIC_HOST");

  /* No explicit newrelic_init(); we'll let the defaults work their magic. */
  newrelic_configure_log("./c_agent.log", NEWRELIC_LOG_DEBUG);

  app = test_app_create_app(license, host, app_name);

  if (!app) {
    printf("Error: Could not Create Application, is a daemon running?");
    return 1;
  }

  while (true) {
    printf("Press Enter to Create a New Transaction, Ctr-C to exit: ");
    getchar();

    printf("Calling newrelic_start_web_transaction \n");
    txn = newrelic_start_web_transaction(app, "veryImportantWebTransaction");
    if (!txn) {
      printf("Could not start Transaction \n");
    } else {
      printf("Started Transaction \n");
    }

    /* End web transaction */
    printf("Calling newrelic_end_transaction \n");
    if (newrelic_end_transaction(&txn)) {
      printf("newrelic_end_transaction returned true \n");
    } else {
      printf("newrelic_end_transaction returned false \n");
    }
  }

  return 0;
}

static newrelic_app_t* test_app_create_app(const char* license,
                                           const char* host,
                                           const char* app_name) {
  newrelic_app_t* app = 0;
  newrelic_app_config_t* config = 0;

  config = newrelic_create_app_config(app_name, license);
  if (NULL != host) {
    strcpy(config->redirect_collector, host);
  }

  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  config->transaction_tracer.duration_us = 1;

  /* Wait up to 10 seconds for the agent to connect to the daemon */
  app = newrelic_create_app(config, 10000);
  newrelic_destroy_app_config(&config);

  return app;
}
