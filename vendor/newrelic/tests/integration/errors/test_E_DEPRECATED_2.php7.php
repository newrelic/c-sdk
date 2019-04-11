<?php

/*DESCRIPTION
The agent should capture and report deprecation warnings.
*/

/*SKIPIF
<?php
if (version_compare(PHP_VERSION, "7.0", "<")) {
  die("skip: PHP 5 not supported\n");
}
*/

/*INI
error_reporting = E_ALL | E_STRICT
*/

/*EXPECT_REGEX
^\s*(PHP )?Deprecated:\s*mktime\(\): You should be using the time\(\) function instead in .*? on line [0-9]+\s*$
*/

/*EXPECT_TRACED_ERRORS
[
  "?? agent run id",
  [
    [
      "?? when",
      "OtherTransaction/php__FILE__",
      "mktime(): You should be using the time() function instead",
      "Error",
      {
        "stack_trace": [
          " in mktime called at __FILE__ (??)"
        ],
        "agentAttributes": "??",
        "intrinsics": "??"
      }
    ]
  ]
]
*/

/*EXPECT_ERROR_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": 1
  },
  [
    [
      {
        "type": "TransactionError",
        "timestamp": "??",
        "error.class": "Error",
        "error.message": "mktime(): You should be using the time() function instead",
        "transactionName": "OtherTransaction\/php__FILE__",
        "duration": "??",
        "nr.transactionGuid": "??"
      },
      {},
      {}
    ]
  ]
]
*/

mktime();
