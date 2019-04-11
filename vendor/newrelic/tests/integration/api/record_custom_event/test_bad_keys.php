<?php

/*DESCRIPTION
Tests newrelic_record_custom_event() with various illegal types of parameter
array keys. I sure wish we had an agent logfile parser that could tells us:
warning: improper parameter array key for newrelic_record_custom_event
*/

/*INI
newrelic.custom_insights_events.enabled = 1
*/

/*EXPECT_CUSTOM_EVENTS
[
  "?? agent run id",
  "?? sampling information",
    [
      [
        {
          "type":"alpha",
          "timestamp":"??"
        },
      {},
      {}
    ],
    [
      {
        "type":"alpha",
        "timestamp":"??"
      },
      {},
      {}
    ],
    [
      {
        "type":"alpha",
        "timestamp":"??"
      },
      {},
      {}
    ]
  ]
]
*/

newrelic_record_custom_event("alpha", array(4=>1));
newrelic_record_custom_event("alpha", array(4.34=>1));
newrelic_record_custom_event("alpha", array(true=>1));
