#define _GNU_SOURCE

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libnewrelic.h"
#include "common.h"

static void test_app_attributes(newrelic_txn_t* txn);
static void test_app_error(newrelic_txn_t* txn);
static void test_app_segments(newrelic_txn_t* txn);
static newrelic_app_t* test_app_create_app(const char* license,
                                           const char* host,
                                           const char* app_name);
static void test_app_start_background_transaction(newrelic_app_t* app);
static void test_app_custom_events(newrelic_txn_t* txn);
static void test_app_custom_metrics(newrelic_txn_t* txn);

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
  newrelic_configure_log("./c_sdk.log", NEWRELIC_LOG_DEBUG);

  app = test_app_create_app(license, host, app_name);

  if (!app) {
    printf("Error: Could not Create Application, is a daemon running?");
    return 1;
  }

  /* Start a web transaction */
  txn = newrelic_start_web_transaction(app, "veryImportantWebTransaction");

  test_app_attributes(txn);
  test_app_error(txn);
  test_app_segments(txn);
  test_app_custom_events(txn);
  test_app_custom_metrics(txn);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  test_app_start_background_transaction(app);

  newrelic_destroy_app(&app);

  return 0;
}

static void test_app_attributes(newrelic_txn_t* txn) {
  /* Add attributes */
  newrelic_add_attribute_int(txn, "my_custom_int", INT_MAX);
  newrelic_add_attribute_string(txn, "my_custom_string",
                                "String String String");

  sleep(1);
}

static void test_app_error(newrelic_txn_t* txn) {
  int priority = 50;

  /* Record an error */
  newrelic_notice_error(txn, priority, "Meaningful error message",
                        "Error.class");
}

static void test_app_segments(newrelic_txn_t* txn) {
  newrelic_segment_t* segment1 = 0;
  newrelic_segment_t* segment2 = 0;

  /* Add segments */
  segment1 = newrelic_start_segment(txn, "Stuff", "Custom");
  sleep(1);
  newrelic_end_segment(txn, &segment1);

  segment2 = newrelic_start_segment(txn, "More Stuff", "Custom");
  sleep(1);
  segment1 = newrelic_start_segment(txn, "Nested Stuff", "Custom");
  sleep(1);
  newrelic_end_segment(txn, &segment1);
  sleep(1);
  newrelic_end_segment(txn, &segment2);
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

static void test_app_start_background_transaction(newrelic_app_t* app) {
  newrelic_txn_t* txn = 0;

  /* Start and end a non-web transaction */
  txn = newrelic_start_non_web_transaction(app,
                                           "veryImportantOtherTransaction");
  sleep(1);
  newrelic_end_transaction(&txn);
}

static void test_app_custom_events(newrelic_txn_t* txn) {
  newrelic_custom_event_t* custom_event = 0;
  /* Record a custom event with each type of attribute available in the API */
  custom_event = newrelic_create_custom_event("aTypeForYourEvent");

  newrelic_custom_event_add_attribute_int(custom_event, "keya", 42);
  newrelic_custom_event_add_attribute_long(custom_event, "keyb", 84);
  newrelic_custom_event_add_attribute_double(custom_event, "keyc", 42.42);
  newrelic_custom_event_add_attribute_string(custom_event, "keyd", "A string");

  newrelic_record_custom_event(txn, &custom_event);
}

static void test_app_custom_metrics(newrelic_txn_t* txn) {
  /* Record a metric value of 100ms in the transaction txn */
  newrelic_record_custom_metric(txn, "Custom/YourMetric/Label", 100);
}
