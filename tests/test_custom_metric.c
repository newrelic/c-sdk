#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "test.h"

static void test_custom_metric_inputs(void** state NRUNUSED) {
  newrelic_txn_t* txn = (newrelic_txn_t*)*state;

  assert_true(newrelic_record_custom_metric(txn, "Metric/URI/Here", 42.47));

  assert_false(newrelic_record_custom_metric(NULL, "Metric/URI/Here", 23.47));

  assert_false(newrelic_record_custom_metric(txn, NULL, 40.12));
}

int main(void) {
  const struct CMUnitTest metric_tests[] = {
      cmocka_unit_test(test_custom_metric_inputs),
  };

  return cmocka_run_group_tests(metric_tests, txn_group_setup,
                                txn_group_teardown);
}
