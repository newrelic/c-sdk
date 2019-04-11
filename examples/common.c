/*!
 * ex_common.c
 *
 * @brief Common function implementations for New Relic C-Agent example code.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"

/*
 * @brief Customize an agent configuration
 *
 * @param [in] config_ptr The address of an agent configuration created using
 * newrelic_create_app_config().
 *
 * @return false if config_ptr or *config_ptr are NULL; true otherwise.
 */
bool customize_config(newrelic_app_config_t** config_ptr) {
  if (NULL != config_ptr && NULL != *config_ptr) {
    char* collector = getenv("NEW_RELIC_HOST");
    newrelic_app_config_t* config = *config_ptr;

    if (NULL != collector) {
      strcpy(config->redirect_collector, collector);
    } else {
      printf("Using default agent configuration for collector...\n");
    }

    return true;
  }
  return false;
}

bool example_init(void) {
  if (!newrelic_configure_log("./c_sdk.log", NEWRELIC_LOG_INFO)) {
    printf("Error configuring logging.\n");
    return false;
  }

  return newrelic_init(NULL, 0);
}

/*!
 * @brief Get the New Relic application name from environment,
 * NEW_RELIC_APP_NAME.
 *
 * @return A pointer to the environment variable NEW_RELIC_APP_NAME; NULL if it
 * is not defined.
 */
char* get_app_name(void) {
  char* app_name = getenv("NEW_RELIC_APP_NAME");

  if (NULL == app_name) {
    printf(ENV_NOTICE);
    printf(
        "\nEnvironment variable NEW_RELIC_APP_NAME must be set to a meaningful "
        "application name.\n");
  }

  return app_name;
}

/*!
 * @brief Get the New Relic license key from environment, NEW_RELIC_LICENSE_KEY.
 *
 * @return A pointer to the environment variable NEW_RELIC_LICENSE_KEY; NULL if
 * it is not defined.
 */
char* get_license_key(void) {
  char* license_key = getenv("NEW_RELIC_LICENSE_KEY");

  if (NULL == license_key) {
    printf(ENV_NOTICE);
    printf(
        "\nEnvironment variable NEW_RELIC_LICENSE_KEY must be set to a valid "
        "New "
        "Relic license key.\n");
  }

  return license_key;
}

/*!
 * @brief Get the current time.
 *
 * @return The current time in microseconds since the UNIX Epoch.
 */
newrelic_time_us_t now_us(void) {
  struct timeval tv;
  newrelic_time_us_t ret;

  (void)gettimeofday(&tv, 0);
  ret = tv.tv_sec * NEWRELIC_MS_PER_SEC;
  ret = ret + tv.tv_usec;
  return ret;
}
