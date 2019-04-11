<?php

/*DESCRIPTION
The agent should send up the host display name as an attribute.
*/

/*INI
newrelic.process_host.display_name = Footastic
*/

/*EXPECT
foo=42
*/

/*EXPECT_ANALYTICS_EVENTS
[
  "??",
  {
    "reservoir_size": "??",
    "events_seen": 1
  },
  [
    [
      {
        "type": "Transaction",
        "name": "OtherTransaction\/php__FILE__",
        "timestamp": "??",
        "duration": "??",
        "totalTime": "??",
        "error": false
      },
      {},
      {
        "host.displayName": "Footastic"
      }
    ]
  ]
]
*/

function foo($n) {
  printf("foo=%d\n", $n);
}

foo(42);
