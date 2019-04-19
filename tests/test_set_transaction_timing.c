#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "libnewrelic.h"
#include "app.h"
#include "transaction.h"
#include "nr_txn.h"
#include "util_memory.h"
#include "test.h"

void __wrap_nr_txn_set_as_web_transaction(nrtxn_t* txn, const char* reason);

/**
 * Purpose: Mock to ensure the nr_txn_set_as_web_transaction
 * gets called for web transactions
 */
void __wrap_nr_txn_set_as_web_transaction(nrtxn_t* txn,
                                          const char* reason NRUNUSED) {
  check_expected(txn);
}

static void test_set_transaction_timing_null_transaction(
    void** state NRUNUSED) {
  bool result;

  result = newrelic_set_transaction_timing(NULL, 123, 456);
  assert_false(result);
}

static void test_set_transaction_timing_null_txn(void** state NRUNUSED) {
  newrelic_txn_t txn = {0};
  bool result;

  result = newrelic_set_transaction_timing(&txn, 123, 456);
  assert_false(result);
}

static void test_set_transaction_timing(void** state) {
  newrelic_txn_t* txn = NULL;
  bool result;

  // fetch our fixture value from the state
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;

  expect_any(__wrap_nr_txn_set_as_web_transaction, txn);
  txn = newrelic_start_transaction(appWithInfo, "TheTransaction", true);

  result = newrelic_set_transaction_timing(txn, 123, 456);
  assert_true(result);

  assert_int_equal(123, txn->txn->abs_start_time);
  assert_int_equal(456, txn->txn->segment_root->stop_time);

  newrelic_end_transaction(&txn);
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  // to run tests we pass cmocka_run_group_tests an
  // array of unit tests.  A unit tests is a named function
  // passed into cmocka_unit_test
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_set_transaction_timing_null_transaction),
      cmocka_unit_test(test_set_transaction_timing_null_txn),
      cmocka_unit_test(test_set_transaction_timing),
  };

  return cmocka_run_group_tests(tests,              // our tests
                                app_group_setup,    // setup fixture
                                app_group_teardown  // teardown fixtures
  );
}
