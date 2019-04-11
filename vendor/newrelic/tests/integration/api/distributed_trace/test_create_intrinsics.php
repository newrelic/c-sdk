<?php
/*DESCRIPTION
Test that a version 0.2 payload is created.
*/

/*INI
newrelic.distributed_tracing_enabled=1
*/

/*EXPECT_ANALYTICS_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": "??"
  },
  [
    [
      {
        "type": "Transaction",
        "name": "OtherTransaction\/php__FILE__",
        "timestamp": "??",
        "duration": "??",
        "totalTime": "??",
        "guid": "??",
        "sampled": true,
        "traceId": "??",
        "priority": "??",
        "error": false
      },
      {},
      {}
    ]
  ]
]
*/

newrelic_create_distributed_trace_payload();
