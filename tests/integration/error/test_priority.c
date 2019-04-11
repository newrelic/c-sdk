/*DESCRIPTION
The agent should report the error with the highest priority.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

/*EXPECT_TRACED_ERRORS
[
  "?? agent run id",
  [
    [
      "?? when",
      "OtherTransaction/Action/basic",
      "High",
      "Exception",
      {
        "stack_trace": "??",
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
        "error.class": "Exception",
        "error.message": "High",
        "transactionName": "OtherTransaction/Action/basic",
        "duration": "??",
        "nr.transactionGuid": "??"
      },
      {},
      {}
    ]
  ]
]
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* s = newrelic_start_segment(txn, "s1", "other");

  newrelic_notice_error(txn, 3, "High", "Exception");
  newrelic_notice_error(txn, 2, "Low", "Exception");

  newrelic_end_segment(txn, &s);
}
