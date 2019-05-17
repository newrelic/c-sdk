/*DESCRIPTION
 Span events should be created and sent. This
 also tests the default value of span events.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 0;
  cfg->distributed_tracing.enabled = true;
*/


/*EXPECT_SPAN_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": 1000,
    "events_seen": 2
  },
  [
    [
      {
        "type": "Span",
        "traceId": "??",
        "transactionId": "??",
        "sampled": true,
        "priority": "??",
        "name": "OtherTransaction\/Action\/basic",
        "guid": "??",
        "timestamp": "??",
        "duration": "??",
        "category": "generic",
        "nr.entryPoint": true
      },
      {},
      {}
    ],
    [
      {
        "type": "Span",
        "traceId": "??",
        "transactionId": "??",
        "sampled": true,
        "priority": "??",
        "name": "External\/example.com\/all",
        "guid": "??",
        "timestamp": "??",
        "duration": "??",
        "category": "http",
        "parentId": "??",
        "span.kind": "client",
        "component": "curl"
      },
      {},
      {
        "http.url": "http:\/\/example.com",
        "http.method": "GET"
      }
    ]
  ]
]
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* external = newrelic_start_external_segment(
      txn,
      &(newrelic_external_segment_params_t){
          .uri = "http://example.com", .procedure = "GET", .library = "curl"});
  newrelic_end_segment(txn, &external);
}
