<?php
/*DESCRIPTION
Transaction tracer detail should be disabled when distributed tracing is 
enabled.
 */

/*INI
newrelic.distributed_tracing_enabled=1
newrelic.transaction_tracer.threshold = 0
newrelic.transaction_tracer.detail = 1
newrelic.cross_application_tracer.enabled = false
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
        "traceId": "??",
        "duration": "??", 
        "transactionId": "??",
        "name": "OtherTransaction\/php__FILE__",
        "guid": "??",
        "type": "Span",
        "category": "generic",
        "priority": "??", 
        "sampled": true,
        "nr.entryPoint": true,
        "timestamp": "??"
      },
      {},
      {}
    ],
    [
      {
        "traceId": "??",
        "duration": "??",
        "transactionId": "??",
        "name": "External\/127.0.0.1\/all",
        "guid": "??",
        "type": "Span",
        "category": "http",
        "priority": "??",
        "parentId": "??",
        "sampled": true,
        "timestamp": "??",
        "span.kind": "client",
        "component": "file_get_contents"
      },
      {},
      {
        "http.url": "??",
        "http.method": "GET"
      }
    ]
  ]
]
*/

/*EXPECT
 */

require_once(realpath(dirname(__FILE__)) . '/../../include/tap.php');
require_once(realpath(dirname(__FILE__)) . '/../../include/config.php');

function main()
{
    $url = "http://" . make_tracing_url(realpath(dirname(__FILE__)) . '/../../include/tracing_endpoint.php');
    file_get_contents($url);
}
main();

