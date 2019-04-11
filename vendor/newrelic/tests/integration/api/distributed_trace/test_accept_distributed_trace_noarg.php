<?php
/*DESCRIPTION
Tests that our new API function properly complain when params are missing.
*/

/*EXPECT
Warning Detected
Warning Detected
*/

$functions = array('newrelic_accept_distributed_trace_payload','newrelic_accept_distributed_trace_payload_httpsafe');
foreach($functions as $function) {
    ob_start();
    $function();
    $contents = ob_get_clean();
    if(strpos($contents, 'Warning:') !== false)
    {
        echo 'Warning Detected',"\n";
    }
}
