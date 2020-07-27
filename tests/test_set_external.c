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
 * Purpose: Test that newrelic_set_segment_external() handles invalid inputs
 * correctly.
 */
static void test_set_external_segment_invalid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* original_segment
      = newrelic_start_segment(txn, NULL, NULL);
  newrelic_external_segment_params_t params = {
      .uri = NULL,
  };

  assert_null(newrelic_set_segment_external(NULL, NULL, NULL));
  assert_null(newrelic_set_segment_external(txn, NULL, NULL));
  assert_null(newrelic_set_segment_external(NULL, original_segment, NULL));
  assert_null(newrelic_set_segment_external(NULL, NULL, &params));
  assert_null(newrelic_set_segment_external(txn, original_segment, NULL));
  assert_null(newrelic_set_segment_external(txn, NULL, &params));
  assert_null(newrelic_set_segment_external(NULL, original_segment, &params));

  /* A NULL uri should also result in a NULL. */
  assert_null(newrelic_set_segment_external(txn, original_segment, &params));

  /* Now we'll test library and procedure values including slashes, which are
   * prohibited because they do terrible things to metric names and APM in
   * turn. */
  params.uri = "https://newrelic.com/";

  params.library = "foo/bar";
  assert_null(newrelic_set_segment_external(txn, original_segment, &params));
  params.library = NULL;

  params.procedure = "foo/bar";
  assert_null(newrelic_set_segment_external(txn, original_segment, &params));
  params.procedure = NULL;
}

/*
 * Purpose: Test that newrelic_set_segment_external() handles valid inputs
 * correctly.
 */
static void test_set_external_segment_valid(void** state) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;
  newrelic_segment_t* original_segment
      = newrelic_start_segment(txn, NULL, NULL);
  newrelic_external_segment_params_t params = {
      .uri = "https://newrelic.com/",
  };
  newrelic_segment_t* segment;

  /* Test without library or procedure. */
  segment = newrelic_set_segment_external(txn, original_segment, &params);
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
  original_segment = newrelic_start_segment(txn, NULL, NULL);
  segment = newrelic_set_segment_external(txn, original_segment, &params);
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
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest external_tests[] = {
      cmocka_unit_test_setup_teardown(test_set_external_segment_valid,
                                      txn_group_setup, txn_group_teardown),
      cmocka_unit_test_setup_teardown(test_set_external_segment_invalid,
                                      txn_group_setup, txn_group_teardown),
  };

  return cmocka_run_group_tests(external_tests, NULL, NULL);
}
