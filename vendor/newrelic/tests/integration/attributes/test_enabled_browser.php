<?php

/*DESCRIPTION
When browser monitoring attributes are enabled, they should show up.
*/

/*INI
newrelic.browser_monitoring.attributes.enabled = true
*/

/*EXPECT
non-empty attributes hash
*/

// the headers must be requested before the footers can be accessed
newrelic_get_browser_timing_header(true);

newrelic_add_custom_parameter("hat", "who");

$footer = newrelic_get_browser_timing_footer(true);
if(strpos($footer, '"atts":"SxUUEFsfFF4CQhENQ0dfDhAcGQ=="')) {
  echo("non-empty attributes hash");
} else {
    echo $footer;
}
