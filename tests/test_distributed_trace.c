#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "transaction.h"
#include "segment.h"

#include "nr_distributed_trace_private.h"
#include "util_strings.h"

#include "test.h"

/* The underlying axiom calls for distributed tracing are well-tested.
 * Rather than mock up all of what is necessary to get an accept()
 * call to actually pass, we just want to affirm that the C SDK wrapper
 * calls nr_txn_accept_distributed_trace_payload under the expected
 * conditions. */
bool __wrap_nr_txn_accept_distributed_trace_payload(nrtxn_t* txn,
                                                    const char* str_payload,
                                                    const char* transport_type);

bool __wrap_nr_txn_accept_distributed_trace_payload(
    nrtxn_t* txn NRUNUSED,
    const char* str_payload NRUNUSED,
    const char* transport_type NRUNUSED) {
  return (bool)mock();
}

/*
 * Purpose: Test that newrelic_create_distributed_trace_payload()
 * handles invalid inputs correctly.
 */
static void test_create_payload_bad_parameters(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to create a distributed trace payload with invalid parameters
   * must yield NULL */
  assert_null(newrelic_create_distributed_trace_payload(NULL, NULL));
  assert_null(newrelic_create_distributed_trace_payload(NULL, s));

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_create_distributed_trace_payload()
 * handles well-formed inputs correctly.
 */
static void test_create_payload(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);
  char* payload;

  char* expected_payload_for_null_segment
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"tx\":\"e10f\"";

  char* expected_payload_for_explicit_segment
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"id\":\"1234\",\"tx\":\"e10f\"";

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;
  s->segment->id = nr_strdup("1234");

  /* Creating a distributed trace payload with well-formed parameters must yield
   * a non-null payload */
  payload = newrelic_create_distributed_trace_payload(txn, NULL);
  assert_true(NULL != payload);
  assert_true(0
              == nr_strncmp(payload, expected_payload_for_null_segment,
                            nr_strlen(expected_payload_for_null_segment)));
  nr_free(payload);

  payload = newrelic_create_distributed_trace_payload(txn, s);
  assert_true(NULL != payload);
  assert_true(0
              == nr_strncmp(payload, expected_payload_for_explicit_segment,
                            nr_strlen(expected_payload_for_explicit_segment)));
  nr_free(payload);

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload()
 * handles invalid inputs correctly.
 */
static void test_accept_payload_bad_parameters(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);
  char* payload
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"pr\":0.00000,"
        "\"sa\":true,\"ti\":1557867530900}}";

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to accept a distributed trace payload with invalid parameters
   * must yield false */
  assert_false(newrelic_accept_distributed_trace_payload(NULL, NULL, NULL));
  assert_false(newrelic_accept_distributed_trace_payload(NULL, payload, NULL));
  assert_false(newrelic_accept_distributed_trace_payload(
      NULL, NULL, NEWRELIC_TRANSPORT_TYPE_HTTP));

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload()
 * handles well-formed inputs correctly.
 */
static void test_accept_payload(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);
  char* payload
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"pr\":0.00000,"
        "\"sa\":true,\"ti\":1557867530900}}";

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Accepting a distributed trace payload with well-formed parameters must
   * yield a true result */
  will_return(__wrap_nr_txn_accept_distributed_trace_payload, true);
  assert_true(newrelic_accept_distributed_trace_payload(
      txn, payload, NEWRELIC_TRANSPORT_TYPE_HTTP));

  /* Accepting a distributed trace payload with a missing transport_type must
   * yield a true result */
  will_return(__wrap_nr_txn_accept_distributed_trace_payload, true);
  assert_true(newrelic_accept_distributed_trace_payload(txn, payload, NULL));

  newrelic_end_segment(txn, &s);
}

int main(void) {
  const struct CMUnitTest distributed_trace_tests[] = {
      cmocka_unit_test(test_create_payload_bad_parameters),
      cmocka_unit_test(test_create_payload),
      cmocka_unit_test(test_accept_payload_bad_parameters),
      cmocka_unit_test(test_accept_payload),
  };

  return cmocka_run_group_tests(distributed_trace_tests, txn_group_setup,
                                txn_group_teardown);
}
