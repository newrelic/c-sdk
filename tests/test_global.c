#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "global.h"
#include "nr_agent.h"
#include "nr_axiom.h"

#include "test.h"

nr_status_t __wrap_nr_agent_initialize_daemon_connection_parameters(
    const char* listen_path,
    int external_port);

int __wrap_nr_agent_try_daemon_connect(int time_limit_ms);

nr_status_t __wrap_nrl_set_log_file(const char* filename);

nr_status_t __wrap_nrl_set_log_level(const char* level);

nr_status_t __wrap_nr_agent_initialize_daemon_connection_parameters(
    const char* listen_path,
    int external_port) {
  check_expected(listen_path);
  check_expected(external_port);

  return (nr_status_t)mock();
}

int __wrap_nr_agent_try_daemon_connect(int time_limit_ms) {
  check_expected(time_limit_ms);

  return (int)mock();
}

nr_status_t __wrap_nrl_set_log_file(const char* filename) {
  check_expected(filename);

  return (bool)mock();
}

nr_status_t __wrap_nrl_set_log_level(const char* level) {
  check_expected(level);

  return (bool)mock();
}

static int setup(void** state NRUNUSED) {
  // teardown() should take of this anyway, but just in case.
  newrelic_log_configured = false;
  nr_agent_applist = NULL;
  return 0;
}

static int teardown(void** state NRUNUSED) {
  newrelic_shutdown();
  return 0;
}

static void test_configure_log(void** state NRUNUSED) {
  newrelic_loglevel_t out_of_bounds_level = NEWRELIC_LOG_DEBUG + 1;

  // Bad inputs.
  assert_false(newrelic_configure_log(NULL, NEWRELIC_LOG_INFO));
  assert_false(newrelic_log_configured);

  assert_false(newrelic_configure_log("/dev/null", out_of_bounds_level));
  assert_false(newrelic_log_configured);

  // Mocked axiom errors.
  expect_string(__wrap_nrl_set_log_file, filename, "/dev/null");
  will_return(__wrap_nrl_set_log_file, NR_FAILURE);
  assert_false(newrelic_configure_log("/dev/null", NEWRELIC_LOG_INFO));
  assert_false(newrelic_log_configured);

  expect_string(__wrap_nrl_set_log_file, filename, "/dev/null");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_FAILURE);
  assert_false(newrelic_configure_log("/dev/null", NEWRELIC_LOG_INFO));
  assert_false(newrelic_log_configured);

  // Success!
  expect_string(__wrap_nrl_set_log_file, filename, "/dev/null");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_SUCCESS);
  assert_true(newrelic_configure_log("/dev/null", NEWRELIC_LOG_INFO));
  assert_true(newrelic_log_configured);
}

static void test_do_init(void** state NRUNUSED) {
  // Implicit log configuration fails.
  expect_string(__wrap_nrl_set_log_file, filename, "stderr");
  will_return(__wrap_nrl_set_log_file, NR_FAILURE);
  assert_false(newrelic_do_init(NULL, 0));
  assert_false(newrelic_log_configured);
  assert_true(NULL == nr_agent_applist);

  // Implicit log configuration succeeds; parameter initialisation fails.
  expect_string(__wrap_nrl_set_log_file, filename, "stderr");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_SUCCESS);
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/tmp/.newrelic.sock");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_FAILURE);
  assert_false(newrelic_do_init(NULL, 0));
  assert_true(newrelic_log_configured);
  assert_true(NULL == nr_agent_applist);

  // Connect fails. (No log function mocking, since that shouldn't happen
  // again.)
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/tmp/.newrelic.sock");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_SUCCESS);
  expect_value(__wrap_nr_agent_try_daemon_connect, time_limit_ms, 10);
  will_return(__wrap_nr_agent_try_daemon_connect, 0);
  assert_false(newrelic_do_init(NULL, 0));
  assert_true(newrelic_log_configured);
  assert_true(NULL == nr_agent_applist);

  // Connect succeeds. (Explicit parameters this time; the NULL and 0 handling
  // were tested by earlier test cases.)
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/dev/null");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_SUCCESS);
  expect_value(__wrap_nr_agent_try_daemon_connect, time_limit_ms, 20);
  will_return(__wrap_nr_agent_try_daemon_connect, 1);
  assert_true(newrelic_do_init("/dev/null", 20));
  assert_true(newrelic_log_configured);
  assert_true(NULL != nr_agent_applist);
}

static void test_ensure_init(void** state NRUNUSED) {
  // test_do_init() handles all the interesting cases; just ensure that the
  // nr_agent_applist check happens correctly. Firstly, normal initialisation.
  expect_string(__wrap_nrl_set_log_file, filename, "stderr");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_SUCCESS);
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/tmp/.newrelic.sock");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_SUCCESS);
  expect_value(__wrap_nr_agent_try_daemon_connect, time_limit_ms, 10);
  will_return(__wrap_nr_agent_try_daemon_connect, 1);
  assert_true(newrelic_ensure_init());
  assert_true(newrelic_log_configured);
  assert_true(NULL != nr_agent_applist);

  // Now, a subsequent call should succeed, but with no calls to mocked
  // functions.
  assert_true(newrelic_ensure_init());
}

static void test_init(void** state NRUNUSED) {
  // test_do_init() handles all the interesting cases; just ensure that the
  // nr_agent_applist check happens correctly. Firstly, normal initialisation.
  expect_string(__wrap_nrl_set_log_file, filename, "stderr");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_SUCCESS);
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/dev/null");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_SUCCESS);
  expect_value(__wrap_nr_agent_try_daemon_connect, time_limit_ms, 20);
  will_return(__wrap_nr_agent_try_daemon_connect, 1);
  assert_true(newrelic_init("/dev/null", 20));
  assert_true(newrelic_log_configured);
  assert_true(NULL != nr_agent_applist);

  // Now, a subsequent call should fail.
  assert_false(newrelic_init("/dev/null", 20));
}

static void test_shutdown(void** state NRUNUSED) {
  // Without initialisation, this should succeed, silently.
  newrelic_shutdown();

  // With initialisation, this should NULL out nr_agent_applist and clear
  // newrelic_log_configured.
  expect_string(__wrap_nrl_set_log_file, filename, "stderr");
  will_return(__wrap_nrl_set_log_file, NR_SUCCESS);
  expect_string(__wrap_nrl_set_log_level, level, "info");
  will_return(__wrap_nrl_set_log_level, NR_SUCCESS);
  expect_string(__wrap_nr_agent_initialize_daemon_connection_parameters,
                listen_path, "/dev/null");
  expect_value(__wrap_nr_agent_initialize_daemon_connection_parameters,
               external_port, 0);
  will_return(__wrap_nr_agent_initialize_daemon_connection_parameters,
              NR_SUCCESS);
  expect_value(__wrap_nr_agent_try_daemon_connect, time_limit_ms, 20);
  will_return(__wrap_nr_agent_try_daemon_connect, 1);
  assert_true(newrelic_init("/dev/null", 20));
  assert_true(newrelic_log_configured);
  assert_true(NULL != nr_agent_applist);

  newrelic_shutdown();

  assert_false(newrelic_log_configured);
  assert_true(NULL == nr_agent_applist);
}

int main(void) {
  const struct CMUnitTest global_tests[] = {
      cmocka_unit_test_setup_teardown(test_configure_log, setup, teardown),
      cmocka_unit_test_setup_teardown(test_do_init, setup, teardown),
      cmocka_unit_test_setup_teardown(test_ensure_init, setup, teardown),
      cmocka_unit_test_setup_teardown(test_init, setup, teardown),
      cmocka_unit_test_setup_teardown(test_shutdown, setup, teardown),
  };

  return cmocka_run_group_tests(global_tests, NULL, NULL);
}
