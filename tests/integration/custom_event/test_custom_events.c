/*DESCRIPTION
Does newrelic_record_custom_event create the world's most simple event.
*/

/*CONFIG

*/

/*EXPECT_CUSTOM_EVENTS
[
  "?? agent run id",
  "?? sampling information",
  [
    [
      {
        "type":"Some Name",
        "timestamp":"??"
      },
      {},
      {}
    ]
  ]
]
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_custom_event_t* custom_event;
  custom_event = newrelic_create_custom_event("Some Name");
  newrelic_record_custom_event(txn, &custom_event);
}
