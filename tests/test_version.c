#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "nr_axiom.h"

static void test_version(void** state) {
  const char* actual = newrelic_version();
  char expected[32];
  FILE* fp = fopen(NR_STR2(C_AGENT_ROOT) "/VERSION", "r");
  int len;

  (void)state;

  if (NULL == fp) {
    fail_msg("Cannot find VERSION at %s/%s", NR_STR2(C_AGENT_ROOT), "VERSION");
  }

  if (NULL == fgets(expected, sizeof(expected), fp)) {
    fail_msg("Cannot read from VERSION");
  }

  len = strlen(expected);
  if ((len > 0) && ('\n' == expected[len - 1])) {
    expected[len - 1] = '\0';
  }

  assert_string_equal(actual, expected);
  fclose(fp);
}

int main(void) {
  const struct CMUnitTest version_tests[] = {
      cmocka_unit_test(test_version),
  };

  return cmocka_run_group_tests(version_tests, NULL, NULL);
}
