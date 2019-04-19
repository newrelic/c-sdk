/*DESCRIPTION
User defined attributes should be added to the transaction.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

/*EXPECT_ANALYTICS_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": 1
  },
  [
    [
      {
        "type": "Transaction",
        "name": "OtherTransaction/Action/basic",
        "timestamp": "??",
        "duration": "??",
        "totalTime": "??",
        "error": false
      },
      {
        "attrInt": 19,
        "attrLong": 23,
        "attrDouble": 3.14159,
        "attrString": "Attribute string."
      },
      "?? agent attributes"
    ]
  ]
]
*/

/*EXPECT_TXN_TRACES
[
  "?? agent run id",
  [
    [
      "?? entry",
      "?? duration",
      "OtherTransaction/Action/basic",
      "<unknown>",
      [
        [
          0, {}, {},
          "?? trace details",
          {
            "userAttributes": {
              "attrString": "Attribute string.",
              "attrDouble": 3.14159,
              "attrLong": 23,
              "attrInt": 19
            },
            "intrinsics": {
              "totalTime": "??",
              "cpu_time": "??",
              "cpu_user_time": "??",
              "cpu_sys_time": "??"
            }
          }
        ],
        "?? string table"
      ],
      "?? txn guid",
      "?? reserved",
      "?? force persist",
      "?? x-ray sessions",
      null
    ]
  ]
]
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* s = newrelic_start_segment(txn, "segment", "other");

  newrelic_add_attribute_int(txn, "attrInt", 19);
  newrelic_add_attribute_long(txn, "attrLong", 23);
  newrelic_add_attribute_double(txn, "attrDouble", 3.14159);
  newrelic_add_attribute_string(txn, "attrString", "Attribute string.");

  newrelic_end_segment(txn, &s);
}
