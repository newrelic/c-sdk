<?php

/*DESCRIPTION
newrelic_get_request_metadata() should return DT headers for use in a request.
 */

/*INI
newrelic.distributed_tracing_enabled=1
*/

/*EXPECT
ok - metadata is an array
ok - metadata has one element
ok - Payload is valid
*/

require_once(realpath(dirname( __FILE__ )).'/../../../include/tap.php');

$metadata = newrelic_get_request_metadata();

tap_assert(is_array($metadata), 'metadata is an array');
tap_equal(1, count($metadata), 'metadata has one element');
tap_equal(1, preg_match('#^[a-zA-Z0-9\=\+/]*$#', $metadata['newrelic']), 'Payload is valid');
