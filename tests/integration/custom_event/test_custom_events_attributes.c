/*DESCRIPTION
Do the newrelic_custom_event_add_attribute_* functions behave as expected.
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
      {
        "a-string": "Forty Two",
        "a-double": 42.40,
        "a-long": 84,
        "an-int": 42
      },
      {}
    ]
  ]
]
*/

/*EXPECT
ok - true == outcome_int
ok - true == outcome_long
ok - true == outcome_double
ok - true == outcome_string
*/
#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_custom_event_t* custom_event;
  bool outcome_int;
  bool outcome_long;
  bool outcome_double;
  bool outcome_string;

  custom_event = newrelic_create_custom_event("Some Name");
  outcome_int
      = newrelic_custom_event_add_attribute_int(custom_event, "an-int", 42);

  outcome_long
      = newrelic_custom_event_add_attribute_long(custom_event, "a-long", 84);

  outcome_double = newrelic_custom_event_add_attribute_double(
      custom_event, "a-double", 42.40);

  outcome_string = newrelic_custom_event_add_attribute_string(
      custom_event, "a-string", "Forty Two");

  newrelic_record_custom_event(txn, &custom_event);

  TAP_ASSERT(true == outcome_int);
  TAP_ASSERT(true == outcome_long);
  TAP_ASSERT(true == outcome_double);
  TAP_ASSERT(true == outcome_string);
}
