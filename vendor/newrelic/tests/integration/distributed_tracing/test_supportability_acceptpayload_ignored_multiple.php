<?php

/*DESCRIPTION
Tests the Supportability metric "Supportability/DistributedTrace/AcceptPayload/Ignored/Multiple"
by calling newrelic_accept_distributed_trace_payload twice.
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
    [{"name":"DurationByCaller/App/432507/4741547/Unknown/all"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"DurationByCaller/App/432507/4741547/Unknown/allOther"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/php__FILE__"},             [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/php__FILE__"},    [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/DistributedTrace/AcceptPayload/Ignored/Multiple"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/DistributedTrace/AcceptPayload/Success"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"TransportDuration/App/432507/4741547/Unknown/all"},
                                                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"TransportDuration/App/432507/4741547/Unknown/allOther"},
                                                          [1, "??", "??", "??", "??", "??"]]
  ]
]
*/

$payload = '{"v":[0,1],"d":{"ty":"App","ac":"432507","ap":"4741547","id":"3925aa3552e648dd","tr":"3925aa3552e648dd","pr":1.82236,"sa":true,"ti":1538512769934,"tk":"310705"}}';
newrelic_accept_distributed_trace_payload($payload);
newrelic_accept_distributed_trace_payload($payload);
