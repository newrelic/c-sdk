/*DESCRIPTION
Tests the Supportability metric "Supportability/DistributedTrace/AcceptPayload/Ignored/Null"
when the payload is NULL.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 0;
  cfg->distributed_tracing.enabled = true;
*/

/*EXPECT_METRICS
[
  "?? agent run id",
  "?? start time",
  "?? stop time",
  [
    [{"name":"DurationByCaller/Unknown/Unknown/Unknown/Unknown/all"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"DurationByCaller/Unknown/Unknown/Unknown/Unknown/allOther"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/Action/basic"},            [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/DistributedTrace/AcceptPayload/Ignored/Null"},
                                                          [1, 0, 0, 0, 0, 0]]
  ]
]
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_accept_distributed_trace_payload(txn, NULL, NULL);
}
