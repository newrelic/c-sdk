#include "nr_axiom.h"

#include <stddef.h>
#include <stdbool.h>

#include "nr_distributed_trace.h"
#include "nr_distributed_trace_private.h"
#include "util_memory.h"
#include "util_object.h"
#include "util_time.h"
#include "util_strings.h"
#include "util_logging.h"

/*
 * Purpose : Helper function to assign a string value to a field.  This function
 *           is handy because nr_strdup() will allocate memory to have a copy
 *           of the value passed in.  That memory needs to be nr_free()'d before
 *           the field can be changed. Secondly nr_strdup() returns an empty
 *           string "" when NULL is passed in as a parameter.  The desired
 *           outcome is for a NULL value to be passed along to the field as-is.
 *
 * Params :  1. The string field to override
 *           2. The string value to copy into field
 *
 * Returns : None
 */
static inline void set_dt_field(char** field, const char* value) {
  nr_free(*field);

  if (!nr_strempty(value)) {
    *field = nr_strdup(value);
  }
}

nr_distributed_trace_t* nr_distributed_trace_create(void) {
  nr_distributed_trace_t* dt;
  dt = (nr_distributed_trace_t*)nr_zalloc(sizeof(nr_distributed_trace_t));

  // Set any non-zero default values here.

  return dt;
}

bool nr_distributed_trace_accept_inbound_payload(nr_distributed_trace_t* dt,
                                                 const nrobj_t* obj_payload,
                                                 const char* transport_type,
                                                 const char** error) {
  const nrobj_t* obj_payload_data;

  if (NULL != *error) {
    return false;
  }

  if (NULL == dt) {
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_EXCEPTION;
    return false;
  }

  if (NULL == obj_payload) {
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION;
    return false;
  }

  obj_payload_data = nro_get_hash_hash(obj_payload, "d", NULL);

  set_dt_field(&dt->inbound.type,
               nro_get_hash_string(obj_payload_data, "ty", NULL));
  set_dt_field(&dt->inbound.account_id,
               nro_get_hash_string(obj_payload_data, "ac", NULL));
  set_dt_field(&dt->inbound.app_id,
               nro_get_hash_string(obj_payload_data, "ap", NULL));
  set_dt_field(&dt->inbound.guid,
               nro_get_hash_string(obj_payload_data, "id", NULL));
  set_dt_field(&dt->inbound.txn_id,
               nro_get_hash_string(obj_payload_data, "tx", NULL));
  set_dt_field(&dt->trace_id,
               nro_get_hash_string(obj_payload_data, "tr", NULL));

  dt->priority = (nr_sampling_priority_t)nro_get_hash_double(obj_payload_data,
                                                             "pr", NULL);
  dt->sampled = nro_get_hash_boolean(obj_payload_data, "sa", NULL);

  // Convert payload timestamp from MS to US.
  dt->inbound.timestamp
      = ((nrtime_t)nro_get_hash_long(obj_payload_data, "ti", NULL))
        * NR_TIME_DIVISOR_MS;

  nr_distributed_trace_inbound_set_transport_type(dt, transport_type);
  dt->inbound.set = true;

  return true;
}

nrobj_t* nr_distributed_trace_convert_payload_to_object(const char* payload,
                                                        const char** error) {
  nrobj_t* obj_payload;
  const nrobj_t* obj_payload_version;
  const nrobj_t* obj_payload_data;
  const char* required_data_fields[] = {"ty", "ac", "ap", "tr", "ti"};
  nr_status_t err;
  nr_status_t tx_err;
  size_t i = 0;

  if (NULL != *error) {
    return NULL;
  }

  if (nr_strempty(payload)) {
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_NULL;
    return NULL;
  }

  obj_payload = nro_create_from_json(payload);

  if (NULL == obj_payload) {
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION;
    return NULL;
  }

  obj_payload_version = nro_get_hash_array(obj_payload, "v", NULL);

  // Version missing
  if (NULL == obj_payload_version) {
    nro_delete(obj_payload);
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION;
    return false;
  }

  // Compare version major
  if (nro_get_array_int(obj_payload_version, 1, NULL)
      > NR_DISTRIBUTED_TRACE_VERSION_MAJOR) {
    nro_delete(obj_payload);
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_MAJOR_VERSION;
    return NULL;
  }

  obj_payload_data = nro_get_hash_hash(obj_payload, "d", NULL);

  // Check that at least one of guid or transactionId are present
  nro_get_hash_string(obj_payload_data, "tx", &tx_err);
  nro_get_hash_string(obj_payload_data, "id", &err);
  if (NR_FAILURE == err && NR_FAILURE == tx_err) {
    nrl_debug(
        NRL_CAT,
        "Inbound distributed tracing payload format invalid. Missing both "
        "guid (d.id) and transactionId (d.tx).");
    *error = NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION;
    nro_delete(obj_payload);
    return NULL;
  }

  // Check required fields for their key presence
  for (i = 0;
       i < sizeof(required_data_fields) / sizeof(required_data_fields[0]);
       i++) {
    nro_get_hash_string(obj_payload_data, required_data_fields[i], &err);
    if (NR_FAILURE == err) {
      nro_get_hash_long(obj_payload_data, required_data_fields[i], &err);
      if (NR_FAILURE == err) {
        nrl_info(NRL_CAT,
                 "Inbound distributed tracing payload format invalid.  Missing "
                 "field '%s'",
                 required_data_fields[i]);
        *error = NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION;
        nro_delete(obj_payload);
        return NULL;
      }
    }
  }

  return obj_payload;
}

void nr_distributed_trace_destroy(nr_distributed_trace_t** ptr) {
  nr_distributed_trace_t* trace = NULL;

  if ((NULL == ptr) || (NULL == *ptr)) {
    return;
  }

  trace = *ptr;
  nr_free(trace->account_id);
  nr_free(trace->app_id);
  nr_free(trace->guid);
  nr_free(trace->txn_id);
  nr_free(trace->trace_id);
  nr_free(trace->trusted_key);

  nr_free(trace->inbound.type);
  nr_free(trace->inbound.app_id);
  nr_free(trace->inbound.account_id);
  nr_free(trace->inbound.transport_type);
  nr_free(trace->inbound.guid);
  nr_free(trace->inbound.txn_id);

  nr_realfree((void**)ptr);
}

const char* nr_distributed_trace_get_account_id(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->account_id;
}

const char* nr_distributed_trace_get_trusted_key(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->trusted_key;
}

const char* nr_distributed_trace_get_app_id(const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->app_id;
}

const char* nr_distributed_trace_get_guid(const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->guid;
}

const char* nr_distributed_trace_get_txn_id(const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->txn_id;
}
nr_sampling_priority_t nr_distributed_trace_get_priority(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NR_PRIORITY_ERROR;
  }

  return dt->priority;
}

const char* nr_distributed_trace_get_trace_id(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->trace_id;
}

bool nr_distributed_trace_is_sampled(const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return false;
  }
  return dt->sampled;
}

bool nr_distributed_trace_inbound_is_set(const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return false;
  }

  return dt->inbound.set;
}

const char* nr_distributed_trace_inbound_get_account_id(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.account_id;
}

const char* nr_distributed_trace_inbound_get_app_id(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.app_id;
}

const char* nr_distributed_trace_inbound_get_guid(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.guid;
}

const char* nr_distributed_trace_inbound_get_txn_id(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.txn_id;
}

const char* nr_distributed_trace_inbound_get_type(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.type;
}

nrtime_t nr_distributed_trace_inbound_get_timestamp_delta(
    const nr_distributed_trace_t* dt,
    nrtime_t txn_start) {
  if (NULL == dt) {
    return 0;
  }

  return nr_time_duration(txn_start, dt->inbound.timestamp);
}

const char* nr_distributed_trace_inbound_get_transport_type(
    const nr_distributed_trace_t* dt) {
  if (NULL == dt) {
    return NULL;
  }

  return dt->inbound.transport_type;
}

const char* nr_distributed_trace_object_get_account_id(const nrobj_t* object) {
  const nrobj_t* obj_payload_data;
  obj_payload_data = nro_get_hash_hash(object, "d", NULL);
  return nro_get_hash_string(obj_payload_data, "ac", NULL);
}

const char* nr_distributed_trace_object_get_trusted_key(const nrobj_t* object) {
  const nrobj_t* obj_payload_data;
  obj_payload_data = nro_get_hash_hash(object, "d", NULL);
  return nro_get_hash_string(obj_payload_data, "tk", NULL);
}

void nr_distributed_trace_set_guid(nr_distributed_trace_t* dt,
                                   const char* guid) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->guid);
  if (guid) {
    dt->guid = nr_strdup(guid);
  }
}

void nr_distributed_trace_set_txn_id(nr_distributed_trace_t* dt,
                                     const char* txn_id) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->txn_id);
  if (txn_id) {
    dt->txn_id = nr_strdup(txn_id);
  }
}

void nr_distributed_trace_set_trusted_key(nr_distributed_trace_t* dt,
                                          const char* trusted_key) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->trusted_key);
  if (trusted_key) {
    dt->trusted_key = nr_strdup(trusted_key);
  }
}

void nr_distributed_trace_set_account_id(nr_distributed_trace_t* dt,
                                         const char* account_id) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->account_id);
  if (account_id) {
    dt->account_id = nr_strdup(account_id);
  }
}

void nr_distributed_trace_set_app_id(nr_distributed_trace_t* dt,
                                     const char* app_id) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->app_id);
  if (app_id) {
    dt->app_id = nr_strdup(app_id);
  }
}

void nr_distributed_trace_set_trace_id(nr_distributed_trace_t* dt,
                                       const char* trace_id) {
  if (NULL == dt) {
    return;
  }

  nr_free(dt->trace_id);
  if (trace_id) {
    dt->trace_id = nr_strdup(trace_id);
  }
}

void nr_distributed_trace_set_priority(nr_distributed_trace_t* dt,
                                       nr_sampling_priority_t priority) {
  if (NULL == dt) {
    return;
  }

  dt->priority = priority;
}

void nr_distributed_trace_set_sampled(nr_distributed_trace_t* dt, bool value) {
  if (NULL == dt) {
    return;
  }
  dt->sampled = value;
}

void nr_distributed_trace_inbound_set_transport_type(nr_distributed_trace_t* dt,
                                                     const char* value) {
  static const char* supported_types[]
      = {"Unknown", "HTTP", "HTTPS", "Kafka", "JMS",
         "IronMQ",  "AMQP", "Queue", "Other"};

  if (NULL == dt) {
    return;
  }

  for (unsigned i = 0; i < sizeof(supported_types) / sizeof(supported_types[0]);
       i++) {
    if (0 == nr_strcmp(value, supported_types[i])) {
      set_dt_field(&dt->inbound.transport_type, value);
      return;
    }
  }

  nrl_verbosedebug(NRL_CAT, "Unknown transport type in %s: %s", __func__,
                   value ? value : "(null)");

  set_dt_field(&dt->inbound.transport_type, "Unknown");
}

nr_distributed_trace_payload_t* nr_distributed_trace_payload_create(
    nr_distributed_trace_t* metadata,
    const char* parent_id) {
  nr_distributed_trace_payload_t* p;
  p = (nr_distributed_trace_payload_t*)nr_zalloc(
      sizeof(nr_distributed_trace_payload_t));

  p->metadata = metadata;
  p->timestamp = nr_get_time();

  if (parent_id) {
    p->parent_id = nr_strdup(parent_id);
  }

  return p;
}

void nr_distributed_trace_payload_destroy(
    nr_distributed_trace_payload_t** ptr) {
  nr_distributed_trace_payload_t* payload;
  if (NULL == ptr || NULL == *ptr) {
    return;
  }

  payload = *ptr;

  nr_free(payload->parent_id);

  nr_realfree((void**)ptr);
}

const char* nr_distributed_trace_payload_get_parent_id(
    const nr_distributed_trace_payload_t* payload) {
  if (NULL == payload) {
    return NULL;
  }

  return payload->parent_id;
}

nrtime_t nr_distributed_trace_payload_get_timestamp(
    const nr_distributed_trace_payload_t* payload) {
  if (NULL == payload) {
    return 0;
  }

  return payload->timestamp;
}

const nr_distributed_trace_t* nr_distributed_trace_payload_get_metadata(
    const nr_distributed_trace_payload_t* payload) {
  if (NULL == payload) {
    return NULL;
  }

  return payload->metadata;
}

static inline void add_field_if_set(nrobj_t* obj,
                                    const char* key,
                                    const char* value) {
  if (value) {
    nro_set_hash_string(obj, key, value);
  }
}

char* nr_distributed_trace_payload_as_text(
    const nr_distributed_trace_payload_t* payload) {
  nrobj_t* data;
  nrobj_t* obj;
  char* text;
  nrobj_t* version;

  if ((NULL == payload) || (NULL == payload->metadata)) {
    return NULL;
  }

  if (NULL == payload->metadata->guid && NULL == payload->metadata->txn_id) {
    return NULL;
  }

  obj = nro_new_hash();

  version = nro_new_array();
  nro_set_array_int(version, 0, NR_DISTRIBUTED_TRACE_VERSION_MAJOR);
  nro_set_array_int(version, 0, NR_DISTRIBUTED_TRACE_VERSION_MINOR);
  nro_set_hash(obj, "v", version);
  nro_delete(version);

  data = nro_new_hash();
  nro_set_hash_string(data, "ty", "App");
  add_field_if_set(data, "ac", payload->metadata->account_id);
  add_field_if_set(data, "ap", payload->metadata->app_id);

  add_field_if_set(data, "id", payload->metadata->guid);
  add_field_if_set(data, "tr", payload->metadata->trace_id);
  add_field_if_set(data, "tx", payload->metadata->txn_id);
  nro_set_hash_double(data, "pr", payload->metadata->priority);
  nro_set_hash_boolean(data, "sa", payload->metadata->sampled);
  nro_set_hash_long(data, "ti",
                    (long)(payload->timestamp / NR_TIME_DIVISOR_MS));

  /*
   * According to the spec the trusted key is relevant only when it differs
   * from the account id.
   */
  if (0
      != nr_strcmp(payload->metadata->trusted_key,
                   payload->metadata->account_id)) {
    add_field_if_set(data, "tk", payload->metadata->trusted_key);
  }
  nro_set_hash(obj, "d", data);
  nro_delete(data);

  text = nro_to_json(obj);
  nro_delete(obj);

  return text;
}
