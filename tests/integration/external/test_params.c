/*DESCRIPTION
newrelic_start_external_segment() should record an external segment with the
given options.
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
    [{"name":"External/all"},                                [1, "??", "??", "??", "??", "??"]],
    [{"name":"External/allOther"},                           [1, "??", "??", "??", "??", "??"]],
    [{"name":"External/example.com/all"},                    [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/Action/basic"},               [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                        [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                   [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"},      [1, "??", "??", "??", "??", "??"]],
    [{"name":"External/example.com/all",
      "scope":"OtherTransaction/Action/basic"},              [1, "??", "??", "??", "??", "??"]]
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
                      "uri": "http://example.com",
                      "library": "curl",
                      "procedure": "GET"
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
          "External/example.com/all"
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

/*EXPECT
ok - external != NULL
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* external = newrelic_start_external_segment(
      txn,
      &(newrelic_external_segment_params_t){
          .uri = "http://example.com", .procedure = "GET", .library = "curl"});
  TAP_ASSERT(external != NULL);
  newrelic_end_segment(txn, &external);
}
