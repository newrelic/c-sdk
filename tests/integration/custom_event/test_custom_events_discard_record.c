/*DESCRIPTION
Tests that a custom event discard followed by an accidental record does not
crash the application, and that no custom event is sent.
*/

/*CONFIG

*/

/*EXPECT_CUSTOM_EVENTS
null
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_custom_event_t* custom_event;
  custom_event = newrelic_create_custom_event("Some Name");
  newrelic_discard_custom_event(&custom_event);
  newrelic_record_custom_event(txn, &custom_event);
}
