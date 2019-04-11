#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "config.h"

#include "nr_txn.h"
#include "test.h"
#include "transaction.h"
#include "util_memory.h"

static void test_notice_error_null_transaction(void** state NRUNUSED) {
  newrelic_notice_error(NULL, 0, "message", "class");
  assert_false(false);  // Just make sure we haven't errored out
}

static void test_notice_error_null_error_msg(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, 0, NULL, "class");
  assert_null(txn->txn->error);
}

static void test_notice_error_empty_error_msg(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, 0, "", "class");
  assert_null(txn->txn->error);
}

static void test_notice_error_null_error_class(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, 0, "message", NULL);
  assert_null(txn->txn->error);
}

static void test_notice_error_empty_error_class(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, 0, "message", "");
  assert_false(txn->txn->error);
}

static void test_notice_error_disabled(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  txn->txn->options.err_enabled = 0;  // Force failure by disabling
  newrelic_notice_error(txn, 0, "message", "class");
  txn->txn->options.err_enabled = 1;  // Undo change
  assert_null(txn->txn->error);
}

static void test_notice_error_recording_disabled(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  txn->txn->status.recording = 0;  // Force failure by turning off recording
  newrelic_notice_error(txn, 0, "message", "class");
  txn->txn->status.recording = 1;  // Undo change
  assert_null(txn->txn->error);
}

static void test_notice_error_success(void** state) {
  int priority = 4;
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, priority, "message", "class");
  assert_non_null(txn->txn->error);
  assert_int_equal(priority, nr_error_priority(txn->txn->error));
  nr_error_destroy(&txn->txn->error);
}

static void test_notice_error_add_lower_priority(void** state) {
  int priority_first = 4;
  int priority_second = 2;
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, priority_first, "message", "class");
  assert_int_equal(priority_first, nr_error_priority(txn->txn->error));
  newrelic_notice_error(txn, priority_second, "message", "class");
  assert_int_equal(priority_first, nr_error_priority(txn->txn->error));
  nr_error_destroy(&txn->txn->error);
}

static void test_notice_error_add_higher_priority(void** state) {
  int priority_first = 4;
  int priority_second = 6;
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;
  newrelic_notice_error(txn, priority_first, "message", "class");
  assert_int_equal(priority_first, nr_error_priority(txn->txn->error));
  newrelic_notice_error(txn, priority_second, "message", "class");
  assert_int_equal(priority_second, nr_error_priority(txn->txn->error));
  nr_error_destroy(&txn->txn->error);
}

int main(void) {
  const struct CMUnitTest notice_error_tests[] = {
      cmocka_unit_test(test_notice_error_null_transaction),
      cmocka_unit_test(test_notice_error_null_error_msg),
      cmocka_unit_test(test_notice_error_empty_error_msg),
      cmocka_unit_test(test_notice_error_null_error_class),
      cmocka_unit_test(test_notice_error_empty_error_class),
      cmocka_unit_test(test_notice_error_disabled),
      cmocka_unit_test(test_notice_error_recording_disabled),
      cmocka_unit_test(test_notice_error_add_lower_priority),
      cmocka_unit_test(test_notice_error_add_higher_priority),
      cmocka_unit_test(test_notice_error_success),
  };

  return cmocka_run_group_tests(notice_error_tests, txn_group_setup,
                                txn_group_teardown);
}
