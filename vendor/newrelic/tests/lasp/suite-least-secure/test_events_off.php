<?php

/*DESCRIPTION
Tests that nothing in the LASP code interfers with turning the custom events
configuration being off. (i.e. no custom events sent)
*/

/*INI
newrelic.custom_insights_events.enabled = 0
*/

/*EXPECT_CUSTOM_EVENTS
null
*/

newrelic_record_custom_event("testType", array("integerParam"=>1,
                                               "floatParam"=>1.25,
                                               "stringParam"=>"toastIsDelicious",
                                               "booleanParam"=>true));
