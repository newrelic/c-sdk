<?php
/*DESCRIPTION
Ensure the transaction event include the error intrinsic and that
the intrinsic's value is true for normal PHP errors.
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
        "errorType": "E_USER_ERROR",
        "errorMessage": "This is an error"
      }
    ]
  ]
]
*/

trigger_error("This is an error", E_USER_ERROR);
