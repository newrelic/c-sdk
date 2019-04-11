#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

#include "libnewrelic.h"
#include "app.h"
#include "global.h"
#include "test.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_strings.h"

bool __wrap_newrelic_ensure_init(const char* daemon_socket, int time_limit_ms);

nr_status_t __wrap_newrelic_connect_app(newrelic_app_t* app,
                                        unsigned short timeout_ms);

bool __wrap_newrelic_ensure_init(const char* daemon_socket NRUNUSED,
                                 int time_limit_ms NRUNUSED) {
  return (bool)mock();
}

nr_status_t __wrap_newrelic_connect_app(newrelic_app_t* app NRUNUSED,
                                        unsigned short timeout_ms NRUNUSED) {
  return (nr_status_t)mock();
}

static int setup(void** state) {
  newrelic_app_config_t* config;
  config = (newrelic_app_config_t*)nr_zalloc(sizeof(newrelic_app_config_t));
  *state = config;
  return 0;  // tells cmocka setup completed, 0==OK
}

static int teardown(void** state) {
  newrelic_app_config_t* config;
  config = (newrelic_app_config_t*)*state;
  newrelic_destroy_app_config(&config);
  newrelic_shutdown();
  return 0;  // tells cmocka teardown completed, 0==OK
}

static void test_create_app_null_config(void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  config = NULL;
  app = NULL;
  app = newrelic_create_app(config, 1000);
  assert_null(app);
}

static void test_create_app_empty_appname(void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  config = (newrelic_app_config_t*)*state;
  app = NULL;

  nr_strxcpy(config->app_name, "", nr_strlen(""));

  app = newrelic_create_app(config, 1000);
  assert_null(app);
}

static void test_create_app_license_key_lengths(void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  config = (newrelic_app_config_t*)*state;

  app = NULL;
  nr_strxcpy(config->app_name, "valid app name", nr_strlen("valid app name"));

  // too short
  nr_strxcpy(config->license_key, "abc", nr_strlen("abc"));
  app = newrelic_create_app(config, 1000);
  assert_null(app);

  // too long
  nr_strxcpy(config->license_key, "toolong-toolong-toolong-toolong-toolong-42",
             nr_strlen("toolong-toolong-toolong-toolong-toolong-42"));
  app = newrelic_create_app(config, 1000);
  assert_null(app);
}

static void test_create_app_newrelic_connect_app_returns_failure(
    void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  config = (newrelic_app_config_t*)*state;

  will_return(__wrap_newrelic_ensure_init, true);
  will_return(__wrap_newrelic_connect_app, NR_FAILURE);

  nr_strxcpy(config->app_name, "valid app name", nr_strlen("valid app name"));

  nr_strxcpy(config->license_key, "0123456789012345678901234567890123456789",
             40);

  app = newrelic_create_app(config, 1000);
  assert_null(app);
}

static void test_create_app_newrelic_init_fails(void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  config = (newrelic_app_config_t*)*state;

  will_return(__wrap_newrelic_ensure_init, false);
  app = NULL;
  nr_strxcpy(config->app_name, "valid app name", nr_strlen("valid app name"));

  nr_strxcpy(config->license_key, "0123456789012345678901234567890123456789",
             40);

  app = newrelic_create_app(config, 1000);
  assert_null(app);
}

static void test_create_app_newrelic_app_correctly_populated(
    void** state NRUNUSED) {
  newrelic_app_config_t* config;
  newrelic_app_t* app;
  newrelic_transaction_tracer_config_t tt_config = {
      .enabled = false,
      .threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION,
      .duration_us = 42,
  };

  config = (newrelic_app_config_t*)*state;

  will_return(__wrap_newrelic_ensure_init, true);
  will_return(__wrap_newrelic_connect_app, NR_SUCCESS);

  nr_strxcpy(config->app_name, "valid app name", nr_strlen("valid app name"));

  nr_strxcpy(config->license_key, "0123456789012345678901234567890123456789",
             40);

  config->transaction_tracer = tt_config;

  app = newrelic_create_app(config, 1000);
  assert_non_null(app);

  /* Ensure configuration items are populated correctly in the app structure. */
  assert_string_equal("valid app name", app->config->app_name);
  assert_string_equal("0123456789012345678901234567890123456789",
                      app->config->license_key);
  assert_memory_equal(&tt_config, &app->config->transaction_tracer,
                      sizeof(tt_config));

  newrelic_destroy_app(&app);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_create_app_null_config),
      cmocka_unit_test(test_create_app_empty_appname),
      cmocka_unit_test(test_create_app_license_key_lengths),
      cmocka_unit_test(test_create_app_newrelic_connect_app_returns_failure),
      cmocka_unit_test(test_create_app_newrelic_init_fails),
      cmocka_unit_test(test_create_app_newrelic_app_correctly_populated),
  };

  return cmocka_run_group_tests(tests, setup, teardown);
}
