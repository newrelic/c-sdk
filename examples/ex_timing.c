#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "libnewrelic.h"

int main(void) {
  newrelic_app_t* app = 0;
  newrelic_txn_t* txn = 0;
  newrelic_app_config_t* config = 0;
  newrelic_segment_t* seg = 0;
  newrelic_segment_t* seg_a = 0;
  newrelic_segment_t* seg_c = 0;

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

  /* Manually retime the transaction with a duration of 2 seconds */
  newrelic_set_transaction_timing(txn, now_us(), 2000000);

  /* Create a custom segment */
  seg = newrelic_start_segment(txn, NULL, NULL);
  sleep(1);
  newrelic_set_segment_timing(seg, 0, 500000);
  newrelic_end_segment(txn, &seg);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  newrelic_destroy_app(&app);

  return 0;
}
