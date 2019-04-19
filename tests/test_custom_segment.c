#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "segment.h"
#include "transaction.h"
#include "util_memory.h"
#include "util_sleep.h"
#include "util_vector.h"

#include "test.h"

/*
 * Purpose: Test that newrelic_start_segment() handles invalid inputs
 * correctly.
 */
static void test_start_segment_invalid(void** state NRUNUSED) {
  const char* name = "bob";

  assert_null(newrelic_start_segment(NULL, NULL, NULL));
  assert_null(newrelic_start_segment(NULL, NULL, name));
  assert_null(newrelic_start_segment(NULL, name, name));
  assert_null(newrelic_start_segment(NULL, name, NULL));
}

/*
 * Purpose: Test that newrelic_start_segment() handles NULL names
 * and categories correctly.
 */
static void test_start_segment_name_cat_null(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;

  newrelic_segment_t* seg = newrelic_start_segment(txn, NULL, NULL);
  assert_string_equal(
      "Custom/Unnamed Segment",
      nr_string_get(txn->txn->trace_strings, seg->segment->name));
  assert_int_equal(NR_SEGMENT_CUSTOM, seg->segment->type);

  newrelic_segment_destroy(&seg);
}

/*
 * Purpose: Test that newrelic_start_segment() handles invalid segment
 * and category names correctly.
 */
static void test_start_segment_name_cat_invalid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;

  newrelic_segment_t* seg = newrelic_start_segment(txn, "a/b", "c");
  assert_string_equal(
      "c/Unnamed Segment",
      nr_string_get(txn->txn->trace_strings, seg->segment->name));
  assert_int_equal(NR_SEGMENT_CUSTOM, seg->segment->type);
  newrelic_segment_destroy(&seg);

  seg = newrelic_start_segment(txn, "a", "b/c");
  assert_string_equal(
      "Custom/a", nr_string_get(txn->txn->trace_strings, seg->segment->name));
  assert_int_equal(NR_SEGMENT_CUSTOM, seg->segment->type);
  newrelic_segment_destroy(&seg);
}

/*
 * Purpose: Test that newrelic_start_segment() stores the name
 * and the transaction.
 */
static void test_start_segment_name_cat_txn(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  const char* name = "bob";
  const char* category = "bee";

  newrelic_segment_t* seg = newrelic_start_segment(txn, name, category);
  assert_string_equal(
      "bee/bob", nr_string_get(txn->txn->trace_strings, seg->segment->name));
  assert_int_equal(NR_SEGMENT_CUSTOM, seg->segment->type);
  assert_ptr_equal(txn->txn, seg->transaction);

  newrelic_segment_destroy(&seg);
}

/*
 * Purpose: Test that newrelic_end_segment() handles invalid inputs
 * correctly.
 */
static void test_end_segment_invalid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* seg = NULL;
  nrtxn_t othertxn;

  assert_null(newrelic_end_segment(NULL, NULL));
  assert_null(newrelic_end_segment(txn, NULL));

  assert_null(newrelic_end_segment(txn, &seg));
  seg = newrelic_start_segment(txn, NULL, NULL);
  assert_null(newrelic_end_segment(NULL, &seg));

  seg = newrelic_start_segment(txn, NULL, NULL);
  seg->transaction = NULL;
  assert_null(newrelic_end_segment(txn, &seg));

  seg = newrelic_start_segment(txn, NULL, NULL);
  seg->transaction = &othertxn;
  assert_null(newrelic_end_segment(txn, &seg));
}

/*
 * Purpose: Test that newrelic_end_segment() frees the segment.
 */
static void test_end_segment_free(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* seg = newrelic_start_segment(txn, NULL, NULL);

  assert_non_null(newrelic_end_segment(txn, &seg));
  assert_null(seg);
}

/*
 * Purpose: Test that newrelic_end_segment() updates metrics
 * and trace nodes in the transaction.
 */
static void test_end_segment_metric_trace(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* seg = newrelic_start_segment(txn, NULL, NULL);
  nr_segment_t* segment = seg->segment;

  assert_non_null(newrelic_end_segment(txn, &seg));
  assert_int_equal(1, nr_vector_size(segment->metrics));
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest segment_tests[] = {
      cmocka_unit_test_setup_teardown(test_start_segment_invalid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_start_segment_name_cat_null,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_start_segment_name_cat_invalid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_start_segment_name_cat_txn,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_end_segment_invalid, txn_group_setup,
                                      txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_end_segment_free, txn_group_setup,
                                      txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_end_segment_metric_trace,
                                      txn_group_setup, txn_group_teardown),
  };

  return cmocka_run_group_tests(segment_tests, NULL, NULL);
}
