/*!
 * @file common.h
 *
 * @brief Common constants and function declarations for New Relic C-Agent
 * example code.
 */
#ifndef LIBNEWRELIC_COMMON_H
#define LIBNEWRELIC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libnewrelic.h"

/* Common constant values */
#define NEWRELIC_MS_PER_SEC (1000000)

#define ENV_NOTICE                                                             \
  ("This example program depends on environment variables NEW_RELIC_APP_NAME " \
   "and NEW_RELIC_LICENSE_KEY.")

/* Common function declarations */
bool customize_config(newrelic_app_config_t** config_ptr);
bool example_init(void);
char* get_app_name(void);
char* get_license_key(void);
newrelic_time_us_t now_us(void);

#ifdef __cplusplus
}
#endif

#endif
