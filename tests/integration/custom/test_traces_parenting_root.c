/*DESCRIPTION
A coordinated use of newrelic_get_transaction_root() and
newrelic_set_segment_parent() must override the default segment parentage for
segment 2.
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
    [{"name":"OtherTransaction/Action/basic"},            [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"},   [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s1"},                                 [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s2"},                                 [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s1",
      "scope":"OtherTransaction/Action/basic"},           [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s2",
      "scope":"OtherTransaction/Action/basic"},           [1, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/C/api/set_segment_parent_root"},  [1, "??", "??", "??", "??", "??"]]
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
                    0, 2000, "`1", "?? node attributes",
                    []
                  ],
                  [
                    500, 1500, "`2", "?? node attributes",
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
          "other/s1",
          "other/s2"
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
  newrelic_segment_t* s1;
  newrelic_segment_t* s2;

  s1 = newrelic_start_segment(txn, "s1", "other");
  s2 = newrelic_start_segment(txn, "s2", "other");

  newrelic_set_segment_timing(s1, 0, 2000 * 1000);
  newrelic_set_segment_timing(s2, 500 * 1000, 1000 * 1000);

  /* Because s2 was created while s1 was active, s1 is its parent.
   * Manually reparent s2 by making its parent the root segment */
  newrelic_set_segment_parent_root(s2);

  newrelic_end_segment(txn, &s2);
  newrelic_end_segment(txn, &s1);
}
