#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "app.h"
#include "test.h"

static void test_destroy_null(void** state NRUNUSED) {
  bool ret;
  ret = newrelic_destroy_app(NULL);
  assert_false(ret);
}

static void test_destroy_null_app(void** state NRUNUSED) {
  bool ret;
  newrelic_app_t* app = 0;
  ret = newrelic_destroy_app(&app);
  assert_false(ret);
}

static void test_destroy_valid_app(void** state NRUNUSED) {
  bool ret;
  newrelic_app_t* app;
  app = (newrelic_app_t*)calloc(1, sizeof(newrelic_app_t));
  ret = newrelic_destroy_app(&app);
  assert_true(ret);
  free(app);
}

int main(void) {
  const struct CMUnitTest app_tests[] = {
      cmocka_unit_test(test_destroy_null),
      cmocka_unit_test(test_destroy_null_app),
      cmocka_unit_test(test_destroy_valid_app),
  };

  return cmocka_run_group_tests(app_tests, NULL, NULL);
}
