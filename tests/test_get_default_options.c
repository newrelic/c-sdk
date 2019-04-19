#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "config.h"

#include "nr_txn.h"
#include "test.h"
#include "util_memory.h"

/*
 * Purpose: Test that affirms the default options returned are expected values.
 */
static void test_get_default_options(void** state NRUNUSED) {
  nrtxnopt_t* options = newrelic_get_default_options();

  nrtxnopt_t* correct = nr_zalloc(sizeof(nrtxnopt_t));
  correct->analytics_events_enabled = true;
  correct->err_enabled = true;
  correct->error_events_enabled = true;
  correct->tt_enabled = true;
  correct->tt_is_apdex_f = true;
  correct->tt_recordsql = NR_SQL_OBFUSCATED;
  correct->tt_slowsql = true;

  correct->ep_threshold = 500 * NR_TIME_DIVISOR_MS;
  correct->ss_threshold = 500 * NR_TIME_DIVISOR_MS;

  correct->instance_reporting_enabled = true;
  correct->database_name_reporting_enabled = true;

  correct->custom_events_enabled = true;
  /* Assert that the true portion of the default options were set accordingly.
   */
  assert_true(nr_txn_cmp_options(options, correct));

  nr_free(options);
  nr_free(correct);
}

/*
 * Purpose: Main entry point (i.e. runs the tests)
 */
int main(void) {
  const struct CMUnitTest options_tests[] = {
      cmocka_unit_test(test_get_default_options),
  };

  return cmocka_run_group_tests(options_tests,  // our tests
                                NULL, NULL);
}
