<?php

/*DESCRIPTION
Tests newrelic_record_custom_event() with an empty event type string.
*/

/*INI
newrelic.custom_insights_events.enabled = 1
*/

/*EXPECT_CUSTOM_EVENTS
null
*/

newrelic_record_custom_event("", array("beta"=>1));
