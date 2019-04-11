/*DESCRIPTION
Affirm that the agent can allocate a string, used it to create a metric,
and then free it before the transaction sends without ill effect.
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
    [{"name":"Custom/Metric/Thing"},                    [1, 1, 1, 1, 1, 1]]
  ]
]
*/

// clang-format on
#include "common.h"
#include "util_memory.h"

RUN_NONWEB_TXN("basic") {
  char* foo = NULL;
  foo = nr_strdup("Custom/Metric/Thing");
  newrelic_record_custom_metric(txn, foo, 1000);
  nr_free(foo);
}
