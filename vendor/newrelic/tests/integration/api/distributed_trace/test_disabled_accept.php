<?php
/*DESCRIPTION
Tests that newrelic_accept_distributed_trace_payload() returns false
when distributed tracing is disabled.
*/

/*INI
newrelic.distributed_tracing_enabled=0
*/

/*EXPECT
ok - Accept Call Ignored
*/

require_once(realpath (dirname ( __FILE__ )) . '/../../../include/tap.php');
$payload = '{"v":[0,1],"d":{"ty":"App","ac":"432507","ap":"4741544","id":"ed8d6baec463a5ca","tr":"ed8d6baec463a5ca","pr":1.23077,"sa":true,"ti":1530547594646}}';
tap_equal(false, newrelic_accept_distributed_trace_payload($payload), 'Accept Call Ignored');
