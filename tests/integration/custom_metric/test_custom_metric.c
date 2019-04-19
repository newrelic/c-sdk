/*DESCRIPTION
Tests the basic min/max/median functionality of a metric.  Do three calls to
newrelic_record_custom_metric create a single metric with the expected values?
Does a fourth call with a new metric name work?
*/

/*CONFIG

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
    [{"name":"Metric/URI/Here"},                        [3,  7.0,  7.0,  1.0,  4.0, 21.0]],
    [{"name":"Second/URI/Here"},                        [1,  2,  2,  2,  2, 4]]
  ]
]
*/

// clang-format on
#include "common.h"

RUN_NONWEB_TXN("basic") {
  // min
  newrelic_record_custom_metric(txn, "Metric/URI/Here", 1000);

  // max
  newrelic_record_custom_metric(txn, "Metric/URI/Here", 4000);

  // median
  newrelic_record_custom_metric(txn, "Metric/URI/Here", 2000);

  // second metric
  newrelic_record_custom_metric(txn, "Second/URI/Here", 2000);
}
