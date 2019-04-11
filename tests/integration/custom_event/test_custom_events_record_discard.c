/*DESCRIPTION
Tests that a custom event record followed by an accidental discard does not
crash the application, and that the event is properly sent.
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
  newrelic_discard_custom_event(&custom_event);
}
