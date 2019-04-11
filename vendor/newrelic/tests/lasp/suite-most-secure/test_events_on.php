<?php

/*DESCRIPTION
Tests newrelic_record_custom_event() sends no events when LASP configuration
indicates custom_events:{enabled:false}, but agent is configured to send events
*/

/*INI
newrelic.custom_insights_events.enabled = 1
*/

/*EXPECT_CUSTOM_EVENTS
null
*/

newrelic_record_custom_event("testType", array("integerParam"=>1,
                                               "floatParam"=>1.25,
                                               "stringParam"=>"toastIsDelicious",
                                               "booleanParam"=>true));
