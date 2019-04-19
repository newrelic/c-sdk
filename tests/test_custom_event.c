#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "custom_event.h"
#include "transaction.h"

#include "test.h"
#include "util_object.h"

/*
 * Purpose: Test that newrelic_create_custom_event and friends handles invalid
 * inputs in a sane way.
 */
static void test_custom_event_inputs(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_custom_event_t* custom_event = NULL;
  newrelic_custom_event_t* null_custom_event = NULL;
  newrelic_custom_event_t** p_custom_event = NULL;

  // NULL event types not allowed
  custom_event = newrelic_create_custom_event(NULL);

  // point at a null event to ensure newrelic_record_custom_event
  // will exit early for both
  p_custom_event = &null_custom_event;

  assert_null(custom_event);
  assert_false(newrelic_custom_event_add_attribute_int(NULL, "i", 42));
  assert_false(newrelic_custom_event_add_attribute_long(NULL, "i", 42));
  assert_false(newrelic_custom_event_add_attribute_double(NULL, "d", 42.42));
  assert_false(newrelic_custom_event_add_attribute_string(NULL, "s", "a"));

  newrelic_record_custom_event(NULL, NULL);
  newrelic_record_custom_event(txn, NULL);
  newrelic_record_custom_event(NULL, &custom_event);
  newrelic_record_custom_event(txn, p_custom_event);

  newrelic_discard_custom_event(NULL);
}

/*
 * Purpose: Test the basic API functions work
 */
static void test_custom_event(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_custom_event_t* custom_event;

  custom_event = newrelic_create_custom_event("Some Name");
  assert_string_equal(custom_event->type, "Some Name");

  assert_true(newrelic_custom_event_add_attribute_int(custom_event, "i", 42));

  assert_true(newrelic_custom_event_add_attribute_long(custom_event, "l", 84));
  assert_true(
      newrelic_custom_event_add_attribute_double(custom_event, "d", 42.42));
  assert_true(
      newrelic_custom_event_add_attribute_string(custom_event, "s", "a"));

  newrelic_record_custom_event(txn, &custom_event);

  assert_null(custom_event);
}

/*
 * Purpose: Test that we can discard an event
 */
static void test_custom_event_discard(void** state NRUNUSED) {
  newrelic_custom_event_t* custom_event;

  custom_event = newrelic_create_custom_event("Some Name");
  assert_string_equal(custom_event->type, "Some Name");

  assert_true(newrelic_custom_event_add_attribute_int(custom_event, "i", 42));

  assert_true(newrelic_custom_event_add_attribute_long(custom_event, "l", 84));
  assert_true(
      newrelic_custom_event_add_attribute_double(custom_event, "d", 42.42));
  assert_true(
      newrelic_custom_event_add_attribute_string(custom_event, "s", "a"));

  newrelic_discard_custom_event(&custom_event);

  assert_null(custom_event);
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest external_tests[] = {
      cmocka_unit_test(test_custom_event_inputs),
      cmocka_unit_test(test_custom_event),
      cmocka_unit_test(test_custom_event_discard),
  };

  return cmocka_run_group_tests(external_tests,  // our tests
                                txn_group_setup, txn_group_teardown);
}
