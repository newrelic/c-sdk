#include "libnewrelic.h"
#include "datastore.h"
#include "segment.h"
#include "stack.h"
#include "transaction.h"

#include "util_logging.h"
#include "util_memory.h"
#include "util_sql.h"
#include "util_strings.h"

newrelic_segment_t* newrelic_start_datastore_segment(
    newrelic_txn_t* transaction,
    const newrelic_datastore_segment_params_t* params) {
  nr_datastore_t ds_type = NR_DATASTORE_OTHER;
  newrelic_segment_t* segment = NULL;

  /* Affirm required function parameters and datastore parameters are not
   * NULL */
  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot start a datastore segment on a NULL transaction");
    return NULL;
  }
  if (NULL == params) {
    nrl_error(NRL_INSTRUMENT, "params cannot be NULL");
    return NULL;
  }
  if (NULL == params->product) {
    nrl_error(NRL_INSTRUMENT, "product param cannot be NULL");
    return NULL;
  }

  /* Perform slash validation on product, collection, operation, and host */
  if (!newrelic_validate_segment_param(params->product, "product")) {
    return NULL;
  }
  if (!newrelic_validate_segment_param(params->collection, "collection")) {
    return NULL;
  }
  if (!newrelic_validate_segment_param(params->operation, "operation")) {
    return NULL;
  }
  if (!newrelic_validate_segment_param(params->host, "host")) {
    return NULL;
  }

  /* Get the datastore product type, since we need it to figure out SQL
   * behaviour. */
  ds_type = nr_datastore_from_string(params->product);

  /* If the ds_type is the default, log that the datastore segment has
   * been created for an unsupported datastore product. Logging this fact here
   * may help to uncover future mysteries in supporting the agent. */
  if (NR_DATASTORE_OTHER == ds_type) {
    nrl_info(NRL_INSTRUMENT, "instrumenting unsupported datastore product");
  }

  /* Actually start the segment. */
  nrt_mutex_lock(&transaction->lock);
  {
    /* Set up the C agent wrapper struct. */
    segment = newrelic_segment_create(transaction->txn);
    if (NULL == segment) {
      goto unlock_and_end;
    }

    /* Set the type of the segment here; when the user ends the segment
     * this value is used to affirm that newrelic_end_datastore_segment is used
     * only on a datastore segment. */
    segment->segment->type = NR_SEGMENT_DATASTORE;

    segment->type.datastore.collection
        = nr_strdup_or(params->collection, "other");
    segment->type.datastore.operation
        = nr_strdup_or(params->operation, "other");

    /* Axiom uses the product supplied by the customer whenever the type is
     * NR_DATASTORE_OTHER */
    segment->type.datastore.type = ds_type;

    /* While it is type-safe to allow an empty-string product parameter, having
     * such mangles some of the New Relic UI.  Check for an empty string and
     * replace it with a sensible default. Otherwise, copy over the supplied
     * product */
    segment->type.datastore.string = nr_strempty(params->product)
                                         ? nr_strdup(NEWRELIC_DATASTORE_OTHER)
                                         : nr_strdup(params->product);

    segment->type.datastore.sql
        = params->query ? nr_strdup(params->query) : NULL;

    /* Build out the appropriate nr_datastore_instance_t.  The axiom calls do
     * the work of taking care that a NULL port_or_path_id value is set to its
     * default value, "unknown" */
    nr_datastore_instance_set_host(&segment->type.datastore.instance,
                                   params->host);
    nr_datastore_instance_set_port_path_or_id(&segment->type.datastore.instance,
                                              params->port_path_or_id);
    nr_datastore_instance_set_database_name(&segment->type.datastore.instance,
                                            params->database_name);

  unlock_and_end:;
  }
  nrt_mutex_unlock(&transaction->lock);

  return segment;
}

void newrelic_destroy_datastore_segment_fields(newrelic_segment_t* segment) {
  nr_free(segment->type.datastore.collection);
  nr_free(segment->type.datastore.operation);
  nr_free(segment->type.datastore.sql);
  nr_free(segment->type.datastore.string);
  nr_datastore_instance_destroy_fields(&segment->type.datastore.instance);
}

bool newrelic_end_datastore_segment(newrelic_segment_t* segment) {
  /* Sanity check that the datastore segment is really an datastore segment. */
  if (nrlikely(NR_SEGMENT_DATASTORE == segment->segment->type)) {
    nr_segment_datastore_params_t params = {
        .operation = segment->type.datastore.operation,
        .collection = segment->type.datastore.collection,
        .instance     = &segment->type.datastore.instance,
        .datastore    = {
          .type = segment->type.datastore.type,
          .string = segment->type.datastore.string
        },
        .sql = {
          .sql = segment->type.datastore.sql,
        },
    };
    nr_segment_datastore_end(segment->segment, &params);
    newrelic_destroy_datastore_segment_fields(segment);
    return true;
  }
  nrl_error(NRL_INSTRUMENT,
            "unexpected datastore segment type: expected %d; got %d",
            (int)NR_SEGMENT_DATASTORE, (int)segment->segment->type);
  return false;
}
