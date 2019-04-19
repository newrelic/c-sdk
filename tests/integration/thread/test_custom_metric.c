/*DESCRIPTION
Record custom metrics in different threads.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

// clang-format off
/*EXPECT_METRICS
[
  "?? agent run id",
  "?? timeframe start",
  "?? timeframe stop",
  [
    [{"name":"OtherTransaction/Action/basic"},          [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                   [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},              [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"}, [1, "??", "??", "??", "??", "??"]],
    [{"name":"Metric/URI/Here"},                        [1000, 500500.0,  500500.0,  1.0, 1000.0, "??"]]
  ]
]
*/
// clang-format on

#include "common.h"
#include <unistd.h>
#include <pthread.h>

struct testdata {
  newrelic_txn_t* txn;
  int duration;
};

static void* create_metric(void* argp) {
  newrelic_txn_t* txn = ((struct testdata*)argp)->txn;
  int duration = ((struct testdata*)argp)->duration;

  newrelic_record_custom_metric(txn, "Metric/URI/Here", duration);

  return NULL;
}

RUN_NONWEB_TXN("basic") {
  pthread_t thread_ids[1000];
  struct testdata data[1000];

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    data[i] = (struct testdata){.txn = txn, .duration = (i + 1) * 1000};
    pthread_create(&thread_ids[i], NULL, create_metric, (void*)&data[i]);
  }

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_join(thread_ids[i], NULL);
  }
}
