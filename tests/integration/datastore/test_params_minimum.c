/*DESCRIPTION
newrelic_start_datastore_segment() should record a datastore segment with the
minimum possible options.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

/*EXPECT_METRICS
[
  "?? agent run id",
  "?? start time",
  "?? stop time",
  [
    [{"name":"Datastore/all"},                                [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/allOther"},                           [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/Other/all"},                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/Other/allOther"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/instance/Other/unknown/unknown"},     [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/operation/Other/other"},              [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/statement/Other/other/other"},        [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/statement/Other/other/other",
      "scope":"OtherTransaction/Action/basic"},               [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                         [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/Action/basic"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                    [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"},       [1, "??", "??", "??", "??", "??"]]
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
          [
            "?? start time", "?? end time", "ROOT", "?? root attributes",
            [
              [
                "?? start time", "?? end time", "`0", "?? node attributes",
                [
                  [
                    "?? start time", "?? end time", "`1",
                    {
                      "host": "unknown",
                      "port_path_or_id": "unknown",
                      "database_name": "unknown"
                    },
                    []
                  ]
                ]
              ]
            ]
          ],
          {
            "intrinsics": {
              "totalTime": "??",
              "cpu_time": "??",
              "cpu_user_time": "??",
              "cpu_sys_time": "??"
            }
          }
        ],
        [
          "OtherTransaction/Action/basic",
          "Datastore/statement/Other/other/other"
        ]
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
  newrelic_segment_t* datastore = newrelic_start_datastore_segment(
      txn, &(newrelic_datastore_segment_params_t){.product = ""});
  newrelic_end_segment(txn, &datastore);
}
