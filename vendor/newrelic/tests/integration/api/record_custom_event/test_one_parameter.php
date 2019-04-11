<?php

/*DESCRIPTION
Tests newrelic_record_custom_event() with one single parameter. We just drop
this, since it looks like array(0=>"justOneParameter") to us, and we don't
accept integer keys.
*/

/*INI
newrelic.custom_insights_events.enabled = 1
*/

/*EXPECT_CUSTOM_EVENTS
null
*/

newrelic_record_custom_event("justOneParameter");
