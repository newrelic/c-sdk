<?php
/*DESCRIPTION
Ensure the transaction event include the error intrinsic and that
the intrinsic's value is true for Exceptions.
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
        "error": true
      },
      {},
      {
        "errorType": "Exception",
        "errorMessage": "Uncaught exception 'Exception' with message 'Hello Exception' in __FILE__:??"
      }
    ]
  ]
]
*/

throw new Exception("Hello Exception");
