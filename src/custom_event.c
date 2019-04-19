#include "libnewrelic.h"
#include "custom_event.h"
#include "transaction.h"
#include "util_object.h"
#include "util_logging.h"

newrelic_custom_event_t* newrelic_create_custom_event(const char* event_type) {
  newrelic_custom_event_t* event;

  // bail if we have an invalid event type
  if (NULL == event_type) {
    nrl_error(NRL_INSTRUMENT, "event_type cannot be NULL");
    return NULL;
  }

  event = nr_malloc(sizeof(newrelic_custom_event_t));

  event->type = nr_strdup(event_type);
  event->attributes = nro_new_hash();

  return event;
}

void newrelic_record_custom_event(newrelic_txn_t* transaction,
                                  newrelic_custom_event_t** event) {
  if (NULL == transaction || NULL == event || NULL == *event) {
    return;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    nr_txn_record_custom_event(transaction->txn, (*event)->type,
                               (*event)->attributes);
  }
  nrt_mutex_unlock(&transaction->lock);

  // free the event after it's been recorded
  newrelic_discard_custom_event(event);
}

void newrelic_discard_custom_event(newrelic_custom_event_t** event) {
  if (NULL == event || NULL == *event) {
    return;
  }

  nr_free((*event)->type);
  nro_delete((*event)->attributes);

  nr_realfree((void**)event);
}

bool newrelic_custom_event_add_attribute_int(newrelic_custom_event_t* event,
                                             const char* key,
                                             int value) {
  if (NULL == event) {
    return false;
  }
  return NR_SUCCESS == nro_set_hash_int(event->attributes, key, value);
}

bool newrelic_custom_event_add_attribute_long(newrelic_custom_event_t* event,
                                              const char* key,
                                              long value) {
  if (NULL == event) {
    return false;
  }
  return NR_SUCCESS == nro_set_hash_long(event->attributes, key, value);
}

bool newrelic_custom_event_add_attribute_double(newrelic_custom_event_t* event,
                                                const char* key,
                                                double value) {
  if (NULL == event) {
    return false;
  }
  return NR_SUCCESS == nro_set_hash_double(event->attributes, key, value);
}

bool newrelic_custom_event_add_attribute_string(newrelic_custom_event_t* event,
                                                const char* key,
                                                const char* value) {
  if (NULL == event) {
    return false;
  }

  return NR_SUCCESS == nro_set_hash_string(event->attributes, key, value);
}
