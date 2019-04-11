#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "config.h"

#include "nr_txn.h"
#include "test.h"
#include "util_memory.h"

#define LICENSE_KEY ("Thisisafortycharacterkeyabcdefghijklmnop")

/*
 * Purpose: Test that affirms the transaction options returned without a
 *          newrelic_app_config_t are the same as the default options.
 */
static void test_get_transaction_options_default(void** state NRUNUSED) {
  nrtxnopt_t* actual = newrelic_get_transaction_options(NULL);
  nrtxnopt_t* expected = newrelic_get_default_options();

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
}

/*
 * Purpose: Test that affirms the transaction options with the transaction
 *          tracer disabled are correct.
 */
static void test_get_transaction_options_tt_disabled(void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.enabled = false;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_enabled = false;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the transaction options with the transaction
 *          tracer enabled and set to apdex mode are correct.
 */
static void test_get_transaction_options_tt_threshold_apdex(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_APDEX_FAILING;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_enabled = true;
  expected->tt_is_apdex_f = true;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the transaction options with the transaction
 *          tracer enabled and set to a duration are correct.
 */
static void test_get_transaction_options_tt_threshold_duration(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  config->transaction_tracer.duration_us = 10;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_enabled = true;
  expected->tt_is_apdex_f = false;
  expected->tt_threshold = 10;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the datastore reporting options with
 * recordsql NEWRELIC_SQL_OFF are correct.
 */
static void test_get_transaction_options_datastore_reporting_recordsql_none(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.datastore_reporting.record_sql = NEWRELIC_SQL_OFF;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_recordsql = NR_SQL_NONE;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the datastore reporting options with
 * recordsql NEWRELIC_SQL_RAW are correct.
 */
static void test_get_transaction_options_datastore_reporting_recordsql_raw(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.datastore_reporting.record_sql = NEWRELIC_SQL_RAW;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_recordsql = NR_SQL_RAW;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the datastore reporting options with bad
 * recordsql are correctly set to the default NR_SQL_OBFUSCATED.
 */
static void test_get_transaction_options_datastore_reporting_recordsql_invalid(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.datastore_reporting.record_sql = 1000;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_recordsql = NR_SQL_OBFUSCATED;

  /* For an invalid setting, assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms that the public-facing
 * datastore_reporting.enabled = false value gets converted to
 * the correct transaction options.
 */
static void test_get_transaction_options_datastore_reporting_enabled(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.datastore_reporting.enabled = false;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->tt_slowsql = false;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms that the public-facing
 * datastore_reporting.threshold_us value gets converted from an
 * uint64_t to a time value.
 */
static void test_get_transaction_options_datastore_reporting_threshold(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->transaction_tracer.datastore_reporting.threshold_us = 10;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->ep_threshold = 10 * NR_TIME_DIVISOR_US;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the datastore tracer options with the
 * instance_reporting unset are correct.
 */
static void test_get_transaction_options_datastore_segment_instance_reporting(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->datastore_tracer.instance_reporting = false;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->instance_reporting_enabled = false;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Test that affirms the datastore tracer options with the
 * database_name_reporting unset are correct.
 */
static void
test_get_transaction_options_datastore_segment_database_name_reporting(
    void** state NRUNUSED) {
  nrtxnopt_t* actual;
  nrtxnopt_t* expected;
  newrelic_app_config_t* config
      = newrelic_create_app_config("app name", LICENSE_KEY);

  config->datastore_tracer.database_name_reporting = false;

  actual = newrelic_get_transaction_options(config);
  expected = newrelic_get_default_options();

  expected->database_name_reporting_enabled = false;

  /* Assert that the options were set accordingly. */
  assert_true(nr_txn_cmp_options(actual, expected));

  nr_free(actual);
  nr_free(expected);
  newrelic_destroy_app_config(&config);
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest options_tests[] = {
      cmocka_unit_test(test_get_transaction_options_default),
      cmocka_unit_test(test_get_transaction_options_tt_disabled),
      cmocka_unit_test(test_get_transaction_options_tt_threshold_apdex),
      cmocka_unit_test(test_get_transaction_options_tt_threshold_duration),
      cmocka_unit_test(
          test_get_transaction_options_datastore_reporting_recordsql_none),
      cmocka_unit_test(
          test_get_transaction_options_datastore_reporting_recordsql_raw),
      cmocka_unit_test(
          test_get_transaction_options_datastore_reporting_recordsql_invalid),
      cmocka_unit_test(
          test_get_transaction_options_datastore_reporting_threshold),
      cmocka_unit_test(
          test_get_transaction_options_datastore_reporting_enabled),
      cmocka_unit_test(
          test_get_transaction_options_datastore_segment_instance_reporting),
      cmocka_unit_test(
          test_get_transaction_options_datastore_segment_database_name_reporting),
  };

  return cmocka_run_group_tests(options_tests, NULL, NULL);
}
