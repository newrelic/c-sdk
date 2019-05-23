/*DESCRIPTION
Tests the Supportability metric (specifically for httpsafe function) "Supportability/DistributedTrace/AcceptPayload/Ignored/UntrustedAccount"
by changing the trusted key to an incorrect key "tk":"12345"
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
    [{"name":"OtherTransactionTotalTime/Action/basic"},   [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/DistributedTrace/AcceptPayload/Ignored/UntrustedAccount"},
                                                          [1, "??", "??", "??", "??", "??"]]
  ]
]
*/


#include "common.h"

RUN_NONWEB_TXN("basic") {
  char* payload = "eyJ2IjpbMCwxXSwiZCI6eyJ0eSI6IkFwcCIsImFjIjoiNDMyNTA3IiwiYXAiOiI0NzQxNTQ3IiwiaWQiOiIzOTI1YWEzNTUyZTY0OGRkIiwidHIiOiIzOTI1YWEzNTUyZTY0OGRkIiwicHIiOjEuODIyMzYsInNhIjp0cnVlLCJ0aSI6MTUzODUxMjc2OTkzNCwidGsiOiIxMjM0NTYifX0=";
  newrelic_accept_distributed_trace_payload_httpsafe(txn, payload, NULL);
}
