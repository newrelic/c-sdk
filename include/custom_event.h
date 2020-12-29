/*!
 * @file custom_event.h
 *
 * @brief Type definitions, constants, and function declarations necessary to
 * support adding custom events to transactions in the C SDK.
 */
#ifndef LIBNEWRELIC_CUSTOM_EVENT_H
#define LIBNEWRELIC_CUSTOM_EVENT_H

#include "util_object.h"

/*!
 * @brief The internal custom event struct
 */
typedef struct _newrelic_custom_event_t {
  char* type;
  nrobj_t* attributes;
} newrelic_custom_event_t;

#endif /* LIBNEWRELIC_CUSTOM_EVENT_H */
