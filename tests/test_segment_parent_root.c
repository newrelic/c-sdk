#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "libnewrelic.h"
#include "app.h"
#include "segment.h"
#include "transaction.h"
#include "nr_txn.h"
#include "util_memory.h"
#include "test.h"

static void test_set_segment_parent_root(void** state) {
  newrelic_txn_t* txn = NULL;

  nrtxn_t* saved_txn;
  nr_segment_t* saved_segment;

  newrelic_segment_t* broken;
  newrelic_segment_t* also_broken;

  // fetch our fixture value from the state
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;

  txn = newrelic_start_transaction(appWithInfo, "TheTransaction", true);

  /*
   * Mock up some ill-formed segments to make sure that the
   * function under test is hardened against them.
   */
  broken = newrelic_start_segment(txn, NULL, NULL);
  also_broken = newrelic_start_segment(txn, NULL, NULL);

  saved_txn = broken->transaction;
  broken->transaction = NULL;

  saved_segment = also_broken->transaction->segment_root;
  also_broken->transaction->segment_root = NULL;

  assert_false(newrelic_set_segment_parent_root(NULL));
  assert_false(newrelic_set_segment_parent_root(broken));
  assert_false(newrelic_set_segment_parent_root(also_broken));

  /*
   * Reassemble broken segments before ending them
   */
  broken->transaction = saved_txn;
  also_broken->transaction->segment_root = saved_segment;

  newrelic_end_segment(txn, &also_broken);
  newrelic_end_segment(txn, &broken);
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
      cmocka_unit_test(test_set_segment_parent_root),
  };

  return cmocka_run_group_tests(tests,              // our tests
                                app_group_setup,    // setup fixture
                                app_group_teardown  // teardown fixtures
  );
}
