#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "stack.h"
#include "util_memory.h"
#include "util_object.h"

#include "test.h"

/*
 * Purpose: Tests that our backtrace look like a JSON string.
 *
 * Since a backtrace's contents will vary platform to platform,
 * this test does some basic checks to make sure the string returned
 * looks like a JSON array.
 *
 */
static void test_backtrace_is_jsonish(void** state NRUNUSED) {
  char* stacktrace_json;
  char* stacktrace_json2;
  nrobj_t* obj;

  stacktrace_json = newrelic_get_stack_trace_as_json();

  // does the string begining and end look like JSON
  assert_true(stacktrace_json[0] == '[');
  assert_true(stacktrace_json[1] == '"');
  assert_true(stacktrace_json[strlen(stacktrace_json) - 1] == ']');
  assert_true(stacktrace_json[strlen(stacktrace_json) - 2] == '"');

  // are we bigger than an empty JSON string
  assert_true(strlen(stacktrace_json) > 4);

  // does the string survive a trip through a JSON parsing routine
  obj = nro_create_from_json_unterminated(stacktrace_json,
                                          strlen(stacktrace_json));
  stacktrace_json2 = nro_to_json(obj);
  assert_string_equal(stacktrace_json2, stacktrace_json);

  // free 'em up
  nr_free(stacktrace_json);
  nro_delete(obj);
  nr_free(stacktrace_json2);
}

#ifndef HAVE_BACKTRACE
static void test_no_backtrace_platform(void** state NRUNUSED) {
  char* stacktrace_json;
  stacktrace_json = newrelic_get_stack_trace_as_json();
  assert_string_equal(stacktrace_json, "[\"No backtrace on this platform.\"]");
  nr_free(stacktrace_json);
}
#else
static void test_no_backtrace_platform(void** state NRUNUSED) {
  // fake pass for platforms where backtrace is defined
  assert_true(4 == 4);
}
#endif

int main(void) {
  const struct CMUnitTest license_tests[] = {
      cmocka_unit_test(test_backtrace_is_jsonish),
      cmocka_unit_test(test_no_backtrace_platform),
  };

  return cmocka_run_group_tests(license_tests, NULL, NULL);
}
