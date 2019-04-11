#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "libnewrelic.h"

int main(void) {
  newrelic_app_t* app = 0;
  newrelic_txn_t* txn = 0;
  newrelic_app_config_t* config = 0;
  newrelic_custom_event_t* custom_event = 0;
  newrelic_segment_t* seg = 0;

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

  seg = newrelic_start_segment(txn, NULL, NULL);
  sleep(1);

  /* Record a metric value of 100ms in the transaction txn */
  newrelic_record_custom_metric(txn, "Custom/YourMetric/Label", 100);

  /* Record a custom event with each type of attribute available in the API */
  custom_event = newrelic_create_custom_event("aTypeForYourEvent");

  newrelic_custom_event_add_attribute_int(custom_event, "keya", 42);
  newrelic_custom_event_add_attribute_long(custom_event, "keyb", 84);
  newrelic_custom_event_add_attribute_double(custom_event, "keyc", 42.42);
  newrelic_custom_event_add_attribute_string(custom_event, "keyd", "A string");

  newrelic_record_custom_event(txn, &custom_event);

  /* End the one and only segment */
  newrelic_end_segment(txn, &seg);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  newrelic_destroy_app(&app);

  return 0;
}
