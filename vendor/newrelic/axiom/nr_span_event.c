#include "nr_axiom.h"

#include "nr_span_event.h"
#include "nr_span_event_private.h"
#include "util_memory.h"
#include "util_strings.h"

nr_span_event_t* nr_span_event_create() {
  nr_span_event_t* se;
  se = (nr_span_event_t*)nr_zalloc(sizeof(nr_span_event_t));
  se->type = NR_SPAN_GENERIC;
  return se;
}

void nr_span_event_destroy(nr_span_event_t** ptr) {
  nr_span_event_t* event = NULL;

  if ((NULL == ptr) || (NULL == *ptr)) {
    return;
  }

  event = *ptr;
  nr_free(event->guid);
  nr_free(event->transaction_id);
  nr_free(event->name);
  nr_free(event->datastore.component);
  nr_free(event->datastore.db_statement);
  nr_free(event->datastore.db_instance);
  nr_free(event->datastore.peer_address);
  nr_free(event->datastore.peer_hostname);
  nr_free(event->external.component);
  nr_free(event->external.method);
  nr_free(event->external.url);

  nr_realfree((void**)ptr);
}

const char* nr_span_event_get_guid(const nr_span_event_t* event) {
  if (NULL == event) {
    return NULL;
  }

  return event->guid;
}

const nr_span_event_t* nr_span_event_get_parent(const nr_span_event_t* event) {
  if (NULL == event) {
    return NULL;
  }
  return event->parent;
}

const char* nr_span_event_get_transaction_id(const nr_span_event_t* event) {
  if (NULL == event) {
    return NULL;
  }

  return event->transaction_id;
}

const char* nr_span_event_get_name(const nr_span_event_t* event) {
  if (NULL == event) {
    return NULL;
  }

  return event->name;
}

nr_span_category_t nr_span_event_get_category(const nr_span_event_t* event) {
  if (NULL == event) {
    return 0;
  }

  return event->type;
}

nrtime_t nr_span_event_get_timestamp(const nr_span_event_t* event) {
  if (NULL == event) {
    return 0;
  }

  return event->timestamp;
}

nrtime_t nr_span_event_get_duration(const nr_span_event_t* event) {
  if (NULL == event) {
    return 0;
  }

  return event->duration;
}

bool nr_span_event_is_entry_point(const nr_span_event_t* event) {
  if (NULL == event) {
    return false;
  }

  return event->is_entry_point;
}

const char* nr_span_event_get_external(const nr_span_event_t* event,
                                       nr_span_event_exernal_member_t member) {
  if (NULL == event) {
    return NULL;
  }

  switch (member) {
    case NR_SPAN_EXTERNAL_COMPONENT:
      return event->external.component;
    case NR_SPAN_EXTERNAL_METHOD:
      return event->external.method;
    case NR_SPAN_EXTERNAL_URL:
      return event->external.url;
    default:
      return NULL;
  }
}

const char* nr_span_event_get_datastore(
    const nr_span_event_t* event,
    nr_span_event_datastore_member_t member) {
  if (NULL == event) {
    return NULL;
  }

  switch (member) {
    case NR_SPAN_DATASTORE_COMPONENT:
      return event->datastore.component;
    case NR_SPAN_DATASTORE_DB_STATEMENT:
      return event->datastore.db_statement;
    case NR_SPAN_DATASTORE_DB_INSTANCE:
      return event->datastore.db_instance;
    case NR_SPAN_DATASTORE_PEER_ADDRESS:
      return event->datastore.peer_address;
    case NR_SPAN_DATASTORE_PEER_HOSTNAME:
      return event->datastore.peer_hostname;
    default:
      return NULL;
  }
}

void nr_span_event_set_guid(nr_span_event_t* event, const char* guid) {
  if (NULL == event) {
    return;
  }
  nr_free(event->guid);
  if (guid) {
    event->guid = nr_strdup(guid);
  }
}

void nr_span_event_set_parent(nr_span_event_t* event,
                              const nr_span_event_t* parent_event) {
  if (NULL == event) {
    return;
  }

  event->parent = parent_event;
}

void nr_span_event_set_transaction_id(nr_span_event_t* event,
                                      const char* transaction_id) {
  if (NULL == event) {
    return;
  }

  nr_free(event->transaction_id);
  if (transaction_id) {
    event->transaction_id = nr_strdup(transaction_id);
  }
}

void nr_span_event_set_name(nr_span_event_t* event, const char* name) {
  if (NULL == event) {
    return;
  }

  nr_free(event->name);
  if (name) {
    event->name = nr_strdup(name);
  }
}

void nr_span_event_set_category(nr_span_event_t* event,
                                nr_span_category_t category) {
  if (NULL == event) {
    return;
  }
  event->type = category;
}

void nr_span_event_set_timestamp(nr_span_event_t* event, nrtime_t time) {
  if (NULL == event) {
    return;
  }

  event->timestamp = time;
}

void nr_span_event_set_duration(nr_span_event_t* event, nrtime_t duration) {
  if (NULL == event) {
    return;
  }

  event->duration = duration;
}

void nr_span_event_set_datastore(nr_span_event_t* event,
                                 nr_span_event_datastore_member_t member,
                                 const char* new_value) {
  if (NULL == event) {
    return;
  }

  switch (member) {
    case NR_SPAN_DATASTORE_COMPONENT:
      nr_free(event->datastore.component);
      event->datastore.component = nr_strdup(new_value);
      break;
    case NR_SPAN_DATASTORE_DB_STATEMENT:
      nr_free(event->datastore.db_statement);
      event->datastore.db_statement = nr_strdup(new_value);
      break;
    case NR_SPAN_DATASTORE_DB_INSTANCE:
      nr_free(event->datastore.db_instance);
      event->datastore.db_instance = nr_strdup(new_value);
      break;
    case NR_SPAN_DATASTORE_PEER_ADDRESS:
      nr_free(event->datastore.peer_address);
      event->datastore.peer_address = nr_strdup(new_value);
      break;
    case NR_SPAN_DATASTORE_PEER_HOSTNAME:
      nr_free(event->datastore.peer_hostname);
      event->datastore.peer_hostname = nr_strdup(new_value);
      break;
  }
  return;
}

void nr_span_event_set_external(nr_span_event_t* event,
                                nr_span_event_exernal_member_t member,
                                const char* new_value) {
  if (NULL == event || NULL == new_value) {
    return;
  }

  switch (member) {
    case NR_SPAN_EXTERNAL_URL:
      nr_free(event->external.url);
      event->external.url = nr_strdup(new_value);
      break;
    case NR_SPAN_EXTERNAL_METHOD:
      nr_free(event->external.method);
      event->external.method = nr_strdup(new_value);
      break;
    case NR_SPAN_EXTERNAL_COMPONENT:
      nr_free(event->external.component);
      event->external.component = nr_strdup(new_value);
      break;
  }
}
