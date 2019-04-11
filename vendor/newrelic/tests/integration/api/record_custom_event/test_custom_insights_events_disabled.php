<?php

/*DESCRIPTION
Makes sure the newrelic.custom_insights_events.enabled .ini configuration works.
*/

/*INI
newrelic.custom_insights_events.enabled = 0
*/

/*EXPECT_CUSTOM_EVENTS
null
*/

newrelic_record_custom_event("testType", array("testKey"=>"testValue"));
