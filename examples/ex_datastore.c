#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libnewrelic.h"

int main(void) {
  newrelic_app_t* app = 0;
  newrelic_txn_t* txn = 0;
  newrelic_app_config_t* config = 0;

  example_init();

  char* app_name = get_app_name();
  if (NULL == app_name)
    return -1;

  char* license_key = get_license_key();
  if (NULL == license_key)
    return -1;

  config = newrelic_create_app_config(app_name, license_key);

  customize_config(&config);

  /* Change the transaction tracer threshold to ensure a trace is generated */
  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  config->transaction_tracer.duration_us = 1;

  /* Wait up to 10 seconds for the agent to connect to the daemon */
  app = newrelic_create_app(config, 10000);
  newrelic_destroy_app_config(&config);

  /* Start a web transaction */
  txn = newrelic_start_web_transaction(app, "ExampleWebTransaction");

  /* Create a collection of datastore parameters */
  newrelic_datastore_segment_params_t params
      = {.product = NEWRELIC_DATASTORE_MYSQL,
         .collection = "My_collection",
         .operation = "select",
         .host = "Instance",
         .port_path_or_id = "01",
         .query
         = "SELECT * FROM My_collection WHERE medical_patient is 'Foo Bar' "};

  /* Fake a datastore request by sleeping for two seconds. In a more typical
   * instrumentation scenario the start() and stop() calls for the datastore
   * segment would be before and after code performing an SELECT operation,
   * for example. */
  newrelic_segment_t* segment = newrelic_start_datastore_segment(txn, &params);
  sleep(2);
  newrelic_end_segment(txn, &segment);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  newrelic_destroy_app(&app);

  return 0;
}
