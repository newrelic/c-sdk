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
  newrelic_segment_t* seg = 0;
  newrelic_segment_t* seg_a = 0;
  newrelic_segment_t* seg_c = 0;
  newrelic_segment_t* seg_d = 0;

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

  /* Manually retime the transaction with a duration of 5.5 seconds */
  newrelic_set_transaction_timing(txn, now_us(), 5500000);

  /* Create custom segments */
  seg = newrelic_start_segment(txn, NULL, NULL);
  sleep(1);
  newrelic_end_segment(txn, &seg);

  /* Set up a nested structure of segments, and reparent one of them.
   *
   * A is the parent of B, which by default is the parent of C. However, we
   * will reparent C to be the direct child of A.
   *
   * Note that this means that seg_a must outlive (at least) the call to
   * newrelic_set_segment_parent() for seg_c.
   */
  seg_a = newrelic_start_segment(txn, "A", "Parented by agent");
  sleep(1);

  seg = newrelic_start_segment(txn, "B", "Parented by agent");
  sleep(1);

  seg_c = newrelic_start_segment(txn, "C", "Manually reparented");
  newrelic_set_segment_parent(seg_c, seg_a);

  /* Instead of sleeping, we'll just manually time seg_c and immediately end
   * it. seg_c starts 10 us after the start of the transaction and lasts
   * half a second. */
  newrelic_set_segment_timing(seg_c, 10, 500000);
  newrelic_end_segment(txn, &seg_c);

  /* Create a new segment to be manually re-parented by the top-level segment
   * of the transaction */
  seg_d = newrelic_start_segment(txn, "D", "Manually reparented");
  sleep(1);
  newrelic_set_segment_parent_root(seg_d);

  newrelic_end_segment(txn, &seg_d);
  newrelic_end_segment(txn, &seg);
  newrelic_end_segment(txn, &seg_a);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  newrelic_destroy_app(&app);

  return 0;
}
