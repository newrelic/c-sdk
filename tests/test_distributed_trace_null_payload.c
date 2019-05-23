#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "transaction.h"
#include "nr_distributed_trace_private.h"

#include "test.h"

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload()
 * handles a NULL payload correctly.
 */
static void test_accept_payload_null_payload(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to accept a distributed trace payload with a NULL payload
   * must yield false */
  assert_false(newrelic_accept_distributed_trace_payload(txn, NULL, NULL));
  assert_false(newrelic_accept_distributed_trace_payload(
      txn, NULL, NEWRELIC_TRANSPORT_TYPE_HTTP));

  newrelic_end_segment(txn, &s);
}

/*
 * Purpose: Test that newrelic_accept_distributed_trace_payload_httpsafe()
 * handles a NULL payload correctly.
 */
static void test_accept_payload_httpsafe_null_payload(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* s = newrelic_start_segment(txn, NULL, NULL);

  txn->txn->options.distributed_tracing_enabled = true;
  txn->txn->options.span_events_enabled = true;

  /* Attempting to accept a distributed trace payload with invalid parameters
   * must yield false */
  assert_false(
      newrelic_accept_distributed_trace_payload_httpsafe(txn, NULL, NULL));
  assert_false(newrelic_accept_distributed_trace_payload_httpsafe(
      txn, NULL, NEWRELIC_TRANSPORT_TYPE_HTTP));

  newrelic_end_segment(txn, &s);
}

int main(void) {
  const struct CMUnitTest distributed_trace_tests[] = {
      cmocka_unit_test(test_accept_payload_null_payload),
      cmocka_unit_test(test_accept_payload_httpsafe_null_payload),
  };

  return cmocka_run_group_tests(distributed_trace_tests, txn_group_setup,
                                txn_group_teardown);
}
