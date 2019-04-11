<?php

/*DESCRIPTION
newrelic_get_browser_timing_footer() should return an empty string if
newrelic_get_browser_timing_header() has not been called.
*/

/*EXPECT
ok - first call returns empty string
ok - subsequent calls return empty string
*/

require_once(realpath (dirname ( __FILE__ )) . '/../../../include/tap.php');

$footer = newrelic_get_browser_timing_footer(true);
tap_equal('', $footer, 'first call returns empty string');

$footer2 = newrelic_get_browser_timing_footer(true);
tap_equal('', $footer2, 'subsequent calls return empty string');
