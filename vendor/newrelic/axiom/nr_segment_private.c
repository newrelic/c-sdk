#include "nr_axiom.h"

#include "nr_segment_private.h"
#include "nr_segment.h"
#include "nr_txn.h"
#include "util_memory.h"
#include "util_string_pool.h"
#include "util_time.h"

void nr_segment_datastore_destroy_fields(nr_segment_datastore_t* datastore) {
  if (nrunlikely(NULL == datastore)) {
    return;
  }

  nr_free(datastore->component);
  nr_free(datastore->sql);
  nr_free(datastore->sql_obfuscated);
  nr_free(datastore->input_query_json);
  nr_free(datastore->backtrace_json);
  nr_free(datastore->explain_plan_json);
  nr_free(datastore->instance.host);
  nr_free(datastore->instance.port_path_or_id);
  nr_free(datastore->instance.database_name);
}

void nr_segment_external_destroy_fields(nr_segment_external_t* external) {
  if (nrunlikely(NULL == external)) {
    return;
  }

  nr_free(external->transaction_guid);
  nr_free(external->uri);
  nr_free(external->library);
  nr_free(external->procedure);
}

void nr_segment_destroy_typed_attributes(
    nr_segment_type_t type,
    union _nr_segment_typed_attributes_t* attributes) {
  if (nrunlikely(NULL == attributes)) {
    return;
  }

  if (NR_SEGMENT_DATASTORE == type) {
    nr_segment_datastore_destroy_fields(&attributes->datastore);
  } else if (NR_SEGMENT_EXTERNAL == type) {
    nr_segment_external_destroy_fields(&attributes->external);
  }
}

void nr_segment_destroy_fields(nr_segment_t* segment) {
  if (nrunlikely(NULL == segment)) {
    return;
  }

  nr_free(segment->id);
  nr_vector_destroy(&segment->metrics);
  nr_exclusive_time_destroy(&segment->exclusive_time);
  nro_delete(segment->user_attributes);
  nr_segment_destroy_typed_attributes(segment->type,
                                      &segment->typed_attributes);
}

void nr_segment_metric_destroy_fields(nr_segment_metric_t* sm) {
  nr_free(sm->name);
}
