#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "transaction.h"
#include "nr_distributed_trace_private.h"

#include "util_base64.h"
#include "util_strings.h"

#include "test.h"

/* The underlying axiom calls for distributed tracing are well-tested.
 * Rather than mock up all of what is necessary to get an accept()
 * call to actually pass, we just want to affirm that the C SDK wrapper
 * calls nr_txn_accept_distributed_trace_payload_httpsafe under the expected
 * conditions. */
bool __wrap_nr_txn_accept_distributed_trace_payload_httpsafe(
    nrtxn_t* txn,
    const char* str_payload,
    const char* transport_type);

bool __wrap_nr_txn_accept_distributed_trace_payload_httpsafe(
    nrtxn_t* txn NRUNUSED,
    const char* str_payload NRUNUSED,
    const char* transport_type NRUNUSED) {
  return (bool)mock();
}

/*
 * Purpose: Test that newrelic_create_distributed_trace_payload_httpsafe()
 * handles invalid inputs correctly.
 */
static void test_create_payload_httpsafe_bad_parameters(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to create a distributed trace payload with invalid parameters
   * must yield NULL */
  assert_null(newrelic_create_distributed_trace_payload_httpsafe(NULL, NULL));
  assert_null(newrelic_create_distributed_trace_payload_httpsafe(NULL, s));

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_create_distributed_trace_payload_httpsafe()
 * handles well-formed inputs correctly.
 */
static void test_create_payload_httpsafe(void** state) {
  int decoded_length;
  char* payload;
  char* encoded_payload;
  char* decoded_payload;

  char* timestamp = "\"ti\":1557867530900}}";
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Creating an httpsafe distributed trace payload with well-formed parameters
   * must yield a base64-encoded payload, when decoded, is the same payload
   * created by the plaintext version (except for their timestamps). */
  payload = newrelic_create_distributed_trace_payload(txn, NULL);
  encoded_payload
      = newrelic_create_distributed_trace_payload_httpsafe(txn, NULL);
  decoded_payload = nr_b64_decode(encoded_payload, &decoded_length);
  assert_true(0
              == nr_strncmp(decoded_payload, payload,
                            decoded_length - nr_strlen(timestamp)));

  nr_free(payload);
  nr_free(encoded_payload);
  nr_free(decoded_payload);

  payload = newrelic_create_distributed_trace_payload(txn, s);
  encoded_payload = newrelic_create_distributed_trace_payload_httpsafe(txn, s);
  decoded_payload = nr_b64_decode(encoded_payload, &decoded_length);
  assert_true(0
              == nr_strncmp(decoded_payload, payload,
                            decoded_length - nr_strlen(timestamp)));

  nr_free(payload);
  nr_free(encoded_payload);
  nr_free(decoded_payload);

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload_httpsafe()
 * handles invalid inputs correctly.
 */
static void test_accept_payload_httpsafe_bad_parameters(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  int encoded_len = 0;
  char* encoded_payload;

  char* payload
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"pr\":0.00000,"
        "\"sa\":true,\"ti\":1557867530900}}";

  encoded_payload = nr_b64_encode(payload, nr_strlen(payload), &encoded_len);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to accept a distributed trace payload with invalid parameters
   * must yield false */
  assert_false(
      newrelic_accept_distributed_trace_payload_httpsafe(NULL, NULL, NULL));
  assert_false(newrelic_accept_distributed_trace_payload_httpsafe(
      NULL, encoded_payload, NULL));
  assert_false(newrelic_accept_distributed_trace_payload_httpsafe(
      NULL, NULL, NEWRELIC_TRANSPORT_TYPE_HTTP));

  nr_free(encoded_payload);

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload_httpsafe()
 * handles well-formed inputs correctly.
 */
static void test_accept_payload_httpsafe(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  int encoded_len = 0;
  char* encoded_payload;

  char* payload
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"id\":\"guid\",\"pr\":0.00000,"
        "\"sa\":true,\"ti\":1557867530900}}";

  encoded_payload = nr_b64_encode(payload, nr_strlen(payload), &encoded_len);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Accepting a distributed trace payload with well-formed parameters must
   * yield a true result */
  will_return(__wrap_nr_txn_accept_distributed_trace_payload_httpsafe, true);
  assert_true(newrelic_accept_distributed_trace_payload_httpsafe(
      txn, payload, NEWRELIC_TRANSPORT_TYPE_HTTP));

  /* Accepting a distributed trace payload with a missing transport_type must
   * yield a true result */
  will_return(__wrap_nr_txn_accept_distributed_trace_payload_httpsafe, true);
  assert_true(
      newrelic_accept_distributed_trace_payload_httpsafe(txn, payload, NULL));

  nr_free(encoded_payload);

  newrelic_end_segment(txn, &s);
}

int main(void) {
  const struct CMUnitTest distributed_trace_tests[] = {
      cmocka_unit_test(test_create_payload_httpsafe_bad_parameters),
      cmocka_unit_test(test_create_payload_httpsafe),
      cmocka_unit_test(test_accept_payload_httpsafe_bad_parameters),
      cmocka_unit_test(test_accept_payload_httpsafe),
  };

  return cmocka_run_group_tests(distributed_trace_tests, txn_group_setup,
                                txn_group_teardown);
}
