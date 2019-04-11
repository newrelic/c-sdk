#include "libnewrelic.h"
#include "external.h"
#include "segment.h"
#include "transaction.h"

#include "util_logging.h"
#include "util_memory.h"
#include "util_strings.h"
#include "util_url.h"

newrelic_segment_t* newrelic_start_external_segment(
    newrelic_txn_t* transaction,
    const newrelic_external_segment_params_t* params) {
  newrelic_segment_t* segment = NULL;

  /* Validate our inputs. */
  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot start an external segment on a NULL transaction");
    return NULL;
  }

  if (NULL == params) {
    nrl_error(NRL_INSTRUMENT, "params cannot be NULL");
    return NULL;
  }

  if (!newrelic_validate_segment_param(params->library, "library")) {
    return NULL;
  }

  if (!newrelic_validate_segment_param(params->procedure, "procedure")) {
    return NULL;
  }

  if (NULL == params->uri) {
    nrl_error(NRL_INSTRUMENT, "uri cannot be NULL");
    return NULL;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    segment = newrelic_segment_create(transaction->txn);
    if (NULL == segment) {
      goto unlock_and_end;
    }

    segment->segment->type = NR_SEGMENT_EXTERNAL;

    /* Save the supplied parameters until the external segment is ended */
    segment->type.external.uri = params->uri ? nr_strdup(params->uri) : NULL;
    segment->type.external.library
        = params->library ? nr_strdup(params->library) : NULL;
    segment->type.external.procedure
        = params->procedure ? nr_strdup(params->procedure) : NULL;

  unlock_and_end:;
  }
  nrt_mutex_unlock(&transaction->lock);

  return segment;
}

void newrelic_destroy_external_segment_fields(newrelic_segment_t* segment) {
  nr_free(segment->type.external.uri);
  nr_free(segment->type.external.library);
  nr_free(segment->type.external.procedure);
}

bool newrelic_end_external_segment(newrelic_segment_t* segment) {
  /* Sanity check that the external segment is really an external segment. */
  if (nrlikely(NR_SEGMENT_EXTERNAL == segment->segment->type)) {
    nr_segment_external_params_t params = {
        .uri = segment->type.external.uri,
        .library = segment->type.external.library,
        .procedure = segment->type.external.procedure,
    };
    nr_segment_external_end(segment->segment, &params);
    newrelic_destroy_external_segment_fields(segment);
    return true;
  }
  nrl_error(NRL_INSTRUMENT,
            "unexpected external segment type: expected %d; got %d",
            (int)NR_SEGMENT_EXTERNAL, (int)segment->segment->type);
  return false;
}
