/*DESCRIPTION
newrelic_start_datastore_segment() should record a datastore segment with the
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
    [{"name":"Datastore/all"},                                [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/allOther"},                           [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/MySQL/all"},                          [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/MySQL/allOther"},                     [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/instance/MySQL/p_host/31339"},        [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/operation/MySQL/select"},             [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/statement/MySQL/products/select"},    [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/Action/basic"},                [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                         [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                    [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/Action/basic"},       [1, "??", "??", "??", "??", "??"]],
    [{"name":"Datastore/statement/MySQL/products/select",
      "scope":"OtherTransaction/Action/basic"},               [1, "??", "??", "??", "??", "??"]]
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
                      "host": "p_host",
                      "port_path_or_id": "31339",
                      "database_name": "p_database",
                      "sql_obfuscated": "SELECT * from products"
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
          "Datastore/statement/MySQL/products/select"
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
ok - datastore != NULL
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_segment_t* datastore = newrelic_start_datastore_segment(
      txn, &(newrelic_datastore_segment_params_t){
               .product = NEWRELIC_DATASTORE_MYSQL,
               .collection = "products",
               .operation = "select",
               .host = "p_host",
               .port_path_or_id = "31339",
               .database_name = "p_database",
               .query = "SELECT * from products"});
  TAP_ASSERT(datastore != NULL);
  newrelic_end_segment(txn, &datastore);
}
