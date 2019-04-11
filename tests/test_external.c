#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "external.h"
#include "transaction.h"
#include "util_memory.h"

#include "test.h"

/*
 * Purpose: Test that newrelic_start_external_segment() handles invalid inputs
 * correctly.
 */
static void test_start_external_segment_invalid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_external_segment_params_t params = {
      .uri = NULL,
  };

  assert_null(newrelic_start_external_segment(NULL, NULL));
  assert_null(newrelic_start_external_segment(txn, NULL));
  assert_null(newrelic_start_external_segment(NULL, &params));

  /* A NULL uri should also result in a NULL. */
  assert_null(newrelic_start_external_segment(txn, &params));

  /* Now we'll test library and procedure values including slashes, which are
   * prohibited because they do terrible things to metric names and APM in
   * turn. */
  params.uri = "https://newrelic.com/";

  params.library = "foo/bar";
  assert_null(newrelic_start_external_segment(txn, &params));
  params.library = NULL;

  params.procedure = "foo/bar";
  assert_null(newrelic_start_external_segment(txn, &params));
  params.procedure = NULL;
}

/*
 * Purpose: Test that newrelic_start_external_segment() handles valid inputs
 * correctly.
 */
static void test_start_external_segment_valid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_external_segment_params_t params = {
      .uri = "https://newrelic.com/",
  };
  newrelic_segment_t* segment;

  /* Test without library or procedure. */
  segment = newrelic_start_external_segment(txn, &params);
  assert_non_null(segment);
  assert_int_equal((int)NR_SEGMENT_EXTERNAL, (int)segment->segment->type);
  assert_ptr_equal(txn->txn, segment->transaction);

  /* Ensure the uri was actually copied. */
  assert_string_equal(params.uri, segment->type.external.uri);
  assert_ptr_not_equal(params.uri, segment->type.external.uri);
  assert_null(segment->type.external.library);
  assert_null(segment->type.external.procedure);

  newrelic_segment_destroy(&segment);

  /* Now test with library and procedure. */
  params.library = "curl";
  params.procedure = "GET";
  segment = newrelic_start_external_segment(txn, &params);
  assert_non_null(segment);
  assert_int_equal((int)NR_SEGMENT_EXTERNAL, (int)segment->segment->type);
  assert_ptr_equal(txn->txn, segment->transaction);

  assert_string_equal(params.uri, segment->type.external.uri);
  assert_ptr_not_equal(params.uri, segment->type.external.uri);

  assert_string_equal(params.library, segment->type.external.library);
  assert_ptr_not_equal(params.library, segment->type.external.library);

  assert_string_equal(params.procedure, segment->type.external.procedure);
  assert_ptr_not_equal(params.procedure, segment->type.external.procedure);

  newrelic_segment_destroy(&segment);
}

/*
 * Purpose: Test that newrelic_end_segment() handles invalid inputs
 * correctly.
 */
static void test_end_external_segment_invalid(void** state) {
  newrelic_txn_t other_txn = {0};
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_external_segment_params_t params = {
      .uri = "https://newrelic.com/",
  };
  newrelic_segment_t* segment;

  assert_false(newrelic_end_segment(NULL, NULL));
  assert_false(newrelic_end_segment(txn, NULL));

  /* This should destroy the given segment, even though the segment type is
   * invalid. */
  segment = newrelic_start_external_segment(txn, &params);
  segment->segment->type = NR_SEGMENT_DATASTORE;
  assert_false(newrelic_end_segment(NULL, &segment));
  assert_null(segment);

  /* A different transaction should result in failure, but should still destroy
   * the segment. */
  segment = newrelic_start_external_segment(txn, &params);
  assert_false(newrelic_end_segment(&other_txn, &segment));
  assert_null(segment);
}

/*
 * Purpose: Test that newrelic_end_segment() handles valid inputs
 * correctly.
 */
static void test_end_external_segment_valid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_external_segment_params_t params = {
      .uri = "https://newrelic.com/",
  };
  newrelic_segment_t* segment = newrelic_start_external_segment(txn, &params);
  nr_segment_t* axiom_segment = segment->segment;

  assert_true(newrelic_end_segment(txn, &segment));

  assert_string_equal(
      "External/newrelic.com/all",
      nr_string_get(txn->txn->trace_strings, axiom_segment->name));

  /* Ensure that segment_ptr was freed by checking that it's NULL here (and by
   * checking that the test doesn't leak). */
  assert_null(segment);
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest external_tests[] = {
      cmocka_unit_test_setup_teardown(test_start_external_segment_invalid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_start_external_segment_valid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_end_external_segment_invalid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_end_external_segment_valid,
                                      txn_group_setup, txn_group_teardown),
  };

  return cmocka_run_group_tests(external_tests, NULL, NULL);
}
