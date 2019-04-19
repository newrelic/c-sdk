#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "app.h"
#include "transaction.h"

#include "nr_txn.h"
#include "test.h"
#include "util_memory.h"

/* Declare prototypes for mocks */
void __wrap_nr_txn_set_as_web_transaction(nrtxn_t* txn, const char* reason);

/*
 * Purpose: This is a cmocka mock. It wraps the nr_txn_set_as_web_transaction
 * function.  Instead of calling nr_txn_set_as_web_transaction, our code
 * calls __wrap_nr_txn_set_as_web_transaction.  The mock() function used
 * inside this function returns a queued value.
 */
void __wrap_nr_txn_set_as_web_transaction(nrtxn_t* txn,
                                          const char* reason NRUNUSED) {
  check_expected(txn);
}

/*
 * Purpose: Tests that function can survive a null app being passed
 */
static void test_txn_null_app(void** state NRUNUSED) {
  /* Good enough to check for NULL transaction.  The real test
   * is that nr_txn_set_as_web_transaction never gets called.  If it did
   * the unit test would blow up for not knowing what to return, and we
   * expect this test to fail before ever getting to that step. */
  assert_null(newrelic_start_web_transaction(NULL, "aTransaction"));
}

/*
 * Purpose: Tests that function can survive a null name
 */
static void test_txn_null_name(void** state) {
  newrelic_txn_t* txn = NULL;

  /* fetch our fixture value from the state */
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;

  expect_any(__wrap_nr_txn_set_as_web_transaction, txn);
  txn = newrelic_start_web_transaction(appWithInfo, NULL);

  assert_non_null(txn);
  assert_non_null(txn->txn);
  newrelic_end_transaction(&txn);
}

/*
 * Purpose: Tests that function can survive a null name
 */
static void test_txn_valid(void** state) {
  newrelic_txn_t* txn = NULL;

  /* fetch our fixture value from the state */
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;

  expect_any(__wrap_nr_txn_set_as_web_transaction, txn);
  txn = newrelic_start_web_transaction(appWithInfo, "aTransaction");

  assert_non_null(txn);
  assert_non_null(txn->txn);
  newrelic_end_transaction(&txn);
}

/*
 * Purpose: Tests we can ignore a transaction, test the null case
 */
static void test_txn_ignore(void** state) {
  newrelic_txn_t* txn = NULL;

  /* fetch our fixture value from the state */
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;

  expect_any(__wrap_nr_txn_set_as_web_transaction, txn);
  txn = newrelic_start_web_transaction(appWithInfo, "aTransaction");

  // ensure starting status values are what we think they are
  assert_int_equal(1, txn->txn->status.recording);
  assert_int_equal(0, txn->txn->status.ignore);

  assert_true(newrelic_ignore_transaction(txn));

  assert_int_equal(0, txn->txn->status.recording);
  assert_int_equal(1, txn->txn->status.ignore);

  newrelic_end_transaction(&txn);

  assert_false(newrelic_ignore_transaction(NULL));
}
/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest license_tests[] = {
      cmocka_unit_test(test_txn_null_app),
      cmocka_unit_test(test_txn_null_name),
      cmocka_unit_test(test_txn_valid),
      cmocka_unit_test(test_txn_ignore),
  };

  return cmocka_run_group_tests(license_tests,     /* our tests */
                                app_group_setup,   /* setup fixture */
                                app_group_teardown /* teardown fixtures */
  );
}
