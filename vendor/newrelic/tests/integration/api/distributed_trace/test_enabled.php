<?php
/*DESCRIPTION
Tests that newrelic_create_distributed_trace_payload() returns a valid object
when distributed tracing is enabled.
*/

/*INI
newrelic.distributed_tracing_enabled=1
*/

/*EXPECT
ok - text is not empty
ok - httpSafe is not empty
ok - httpSafe is just the encoded text
*/

require_once(realpath (dirname ( __FILE__ )) . '/../../../include/tap.php');

$payload = newrelic_create_distributed_trace_payload();
$text = $payload->text();
$httpSafe = $payload->httpSafe();

tap_not_equal(0, strlen($text), 'text is not empty');
tap_not_equal(0, strlen($httpSafe), 'httpSafe is not empty');
tap_equal(base64_encode($text), $httpSafe, 'httpSafe is just the encoded text');
