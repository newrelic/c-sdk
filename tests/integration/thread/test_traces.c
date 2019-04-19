/*DESCRIPTION
newrelic_start_segment() adds and reparents segments in threads.
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
    [{"name":"other/parent"},                             [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s1"},				  [5, "??", "??", "??", "??", "??"]],
    [{"name":"other/s2"},				  [5, "??", "??", "??", "??", "??"]],
    [{"name":"other/parent",
      "scope":"OtherTransaction/Action/basic"},           [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/s1",
      "scope":"OtherTransaction/Action/basic"},           [5, "??", "??", "??", "??", "??"]],
    [{"name":"other/s2",
      "scope":"OtherTransaction/Action/basic"},           [5, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/C/api/set_segment_parent"},  [10, "??", "??", "??", "??", "??"]]
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
                    "?? start time", "?? end time", "`1", "?? node attributes",
                    [
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ],
                      [
                        "?? start time", "?? end time", "?? node name", "?? node attributes",
                        []
                      ]
                    ]
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
          "other/parent",
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
#include <unistd.h>
#include <pthread.h>

struct testdata {
  newrelic_txn_t* txn;
  newrelic_segment_t* parent;
};

static void* create_segments(void* argp) {
  newrelic_txn_t* txn = ((struct testdata*)argp)->txn;
  newrelic_segment_t* parent = ((struct testdata*)argp)->parent;
  newrelic_segment_t* s1;
  newrelic_segment_t* s2;

  s1 = newrelic_start_segment(txn, "s1", "other");
  newrelic_set_segment_parent(s1, parent);
  s2 = newrelic_start_segment(txn, "s2", "other");
  newrelic_set_segment_parent(s2, parent);
  usleep(2);
  newrelic_end_segment(txn, &s2);
  newrelic_end_segment(txn, &s1);

  return NULL;
}

RUN_NONWEB_TXN("basic") {
  struct testdata data
      = {.txn = txn, .parent = newrelic_start_segment(txn, "parent", "other")};
  pthread_t thread_ids[5];

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_create(&thread_ids[i], NULL, create_segments, (void*)&data);
  }

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_join(thread_ids[i], NULL);
  }

  newrelic_end_segment(txn, &data.parent);
}
