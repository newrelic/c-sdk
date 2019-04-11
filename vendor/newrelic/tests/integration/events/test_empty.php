<?php

/*DESCRIPTION
The agent should generate a small set of attributes on every transaction.
*/

/*EXPECT_ANALYTICS_EVENTS
 [
  "?? agent run id",
  "?? sampling information",
  [
    [
      {
        "type": "Transaction",
        "name": "OtherTransaction/php__FILE__",
        "timestamp": "??",
        "duration": "??",
        "totalTime": "??",
        "error": false
      },
      {
      },
      {
      }
    ]
  ]
]
*/
