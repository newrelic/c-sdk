#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "attribute.h"
#include "test.h"
#include "nr_txn.h"
#include "util_memory.h"

/* Declare prototypes for mocks */
nr_status_t __wrap_nr_txn_add_user_custom_parameter(nrtxn_t* txn,
                                                    const char* key,
                                                    const nrobj_t* value);

/**
 * Purpose: Mock to catch adding a customer attribute/parameter.  The mock()
 * function used inside this function returns a queued value.
 * The testing programmer (us!) uses the will_return function
 * to queue values (see tests below)
 */
nr_status_t __wrap_nr_txn_add_user_custom_parameter(nrtxn_t* txn NRUNUSED,
                                                    const char* key NRUNUSED,
                                                    const nrobj_t* value
                                                        NRUNUSED) {
  return (nr_status_t)mock();
}

static void test_add_attribute_null_txn(void** state NRUNUSED) {
  bool ret = true;
  nrobj_t* value = nro_new_int(1);
  ret = newrelic_add_attribute(NULL, "key", value);
  nro_delete(value);
  assert_false(ret);
}

static void test_add_attribute_null_key(void** state) {
  bool ret = true;
  newrelic_txn_t* txn;
  nrobj_t* value = nro_new_int(1);
  txn = (newrelic_txn_t*)*state;
  ret = newrelic_add_attribute(txn, NULL, value);
  nro_delete(value);
  assert_false(ret);
}

static void test_add_attribute_null_obj(void** state) {
  bool ret = true;
  newrelic_txn_t* txn;
  txn = (newrelic_txn_t*)*state;
  ret = newrelic_add_attribute(txn, "key", NULL);
  assert_false(ret);
}

static void test_add_attribute_failure(void** state) {
  bool ret = true;
  newrelic_txn_t* txn;
  nrobj_t* value = nro_new_int(1);
  txn = (newrelic_txn_t*)*state;
  will_return(__wrap_nr_txn_add_user_custom_parameter, NR_FAILURE);
  ret = newrelic_add_attribute(txn, "key", value);
  nro_delete(value);
  assert_false(ret);
}

static void test_add_attribute_success(void** state) {
  bool ret = true;
  newrelic_txn_t* txn;
  nrobj_t* value = nro_new_int(1);
  txn = (newrelic_txn_t*)*state;
  will_return(__wrap_nr_txn_add_user_custom_parameter, NR_SUCCESS);
  ret = newrelic_add_attribute(txn, "key", value);
  nro_delete(value);
  assert_true(ret);
}

static void test_add_attribute_string_null_value(void** state) {
  bool ret = true;
  newrelic_txn_t* txn;
  txn = (newrelic_txn_t*)*state;
  ret = newrelic_add_attribute_string(txn, "key", NULL);
  assert_false(ret);
}

int main(void) {
  const struct CMUnitTest attribute_tests[] = {
      cmocka_unit_test(test_add_attribute_null_txn),
      cmocka_unit_test(test_add_attribute_null_key),
      cmocka_unit_test(test_add_attribute_null_obj),
      cmocka_unit_test(test_add_attribute_failure),
      cmocka_unit_test(test_add_attribute_success),
      cmocka_unit_test(test_add_attribute_string_null_value),
  };

  return cmocka_run_group_tests(attribute_tests, txn_group_setup,
                                txn_group_teardown);
}
