/*DESCRIPTION
 Span events should not be created when:
   cfg->distributed_tracing.enabled = false;
   cfg->span_events.enabled = false;
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 0;
  cfg->distributed_tracing.enabled = false;
  cfg->span_events.enabled = false;
*/

/*EXPECT_SPAN_EVENTS*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
newrelic_segment_t* external = newrelic_start_external_segment(
        txn,
        &(newrelic_external_segment_params_t){
                .uri = "http://example.com", .procedure = "GET", .library = "curl"});
newrelic_end_segment(txn, &external);
}
