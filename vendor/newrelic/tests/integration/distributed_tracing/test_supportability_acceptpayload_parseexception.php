<?php

/*DESCRIPTION
Tests the Supportability metric "Supportability/DistributedTrace/AcceptPayload/ParseException"
by removing information from the payload, in this case "ty":"App".
 */

/*INI
newrelic.distributed_tracing_enabled = true
newrelic.cross_application_tracer.enabled = false
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
    [{"name":"OtherTransaction/all"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/php__FILE__"},             [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/php__FILE__"},    [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/DistributedTrace/AcceptPayload/ParseException"},
                                                          [1, "??", "??", "??", "??", "??"]]
  ]
]
*/

$payload = '{"v":[0,1],"d":{"ac":"432507","ap":"4741547","id":"3925aa3552e648dd","tr":"3925aa3552e648dd","pr":1.82236,"sa":true,"ti":1538512769934,"tk":"310705"}}';

newrelic_accept_distributed_trace_payload($payload);
