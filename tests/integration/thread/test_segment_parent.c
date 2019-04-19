/*DESCRIPTION
Reparent segments in different threads.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

// clang-format off
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
    [{"name":"other/child"},                              [10000, "??", "??", "??", "??", "??"]],
    [{"name":"other/parent",
      "scope":"OtherTransaction/Action/basic"},           [1, "??", "??", "??", "??", "??"]],
    [{"name":"other/child",
      "scope":"OtherTransaction/Action/basic"},           [10000, "??", "??", "??", "??", "??"]],
    [{"name":"Supportability/C/api/set_segment_parent"},  [10000, "??", "??", "??", "??", "??"]]
  ]
]
*/
// clang-format on

#include "common.h"
#include <unistd.h>
#include <pthread.h>

struct testdata {
  newrelic_txn_t* txn;
  newrelic_segment_t* parent;
  newrelic_segment_t* child;
};

static void* reparent_segments(void* argp) {
  newrelic_txn_t* txn = ((struct testdata*)argp)->txn;
  newrelic_segment_t* parent = ((struct testdata*)argp)->parent;
  newrelic_segment_t* child = ((struct testdata*)argp)->child;

  newrelic_set_segment_parent(child, parent);

  newrelic_end_segment(txn, &child);

  return NULL;
}

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* parent = newrelic_start_segment(txn, "parent", "other");
  pthread_t thread_ids[10000];
  struct testdata data[10000];

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    data[i] = (struct testdata){
                       .txn = txn,
                       .parent = parent,
                       .child = newrelic_start_segment(txn, "child", "other")};
    pthread_create(&thread_ids[i], NULL, reparent_segments, (void*)&data[i]);
  }

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_join(thread_ids[i], NULL);
  }

  newrelic_end_segment(txn, &parent);
}
