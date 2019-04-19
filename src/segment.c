#include "libnewrelic.h"
#include "datastore.h"
#include "external.h"
#include "global.h"
#include "segment.h"
#include "transaction.h"

#include "util_logging.h"
#include "util_memory.h"
#include "util_strings.h"

#include <stdio.h>

static nrthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

newrelic_segment_t* newrelic_segment_create(nrtxn_t* txn) {
  newrelic_segment_t* segment;
  nr_segment_t* txn_seg;

  txn_seg = nr_segment_start(txn, NULL, NULL);
  if (NULL == txn_seg) {
    return NULL;
  }

  nrt_mutex_lock(&mutex);
  segment = nr_zalloc(sizeof(newrelic_segment_t));
  nrt_mutex_unlock(&mutex);

  segment->transaction = txn;
  segment->segment = txn_seg;

  return segment;
}

void newrelic_segment_destroy(newrelic_segment_t** segment_ptr) {
  newrelic_segment_t* segment;

  if (nrunlikely(NULL == segment_ptr || NULL == *segment_ptr)) {
    return;
  }

  segment = *segment_ptr;
  /* Destroy any type specific fields. */
  if (nrlikely(segment->segment)) {
    switch (segment->segment->type) {
      case NR_SEGMENT_DATASTORE:
        newrelic_destroy_datastore_segment_fields(segment);
        break;
      case NR_SEGMENT_EXTERNAL:
        newrelic_destroy_external_segment_fields(segment);
        break;
      case NR_SEGMENT_CUSTOM:
        /* No special destruction needed */
        break;
      default:
        nrl_error(NRL_INSTRUMENT, "unknown segment type %d",
                  (int)segment->segment->type);
    }
  }

  nr_realfree((void**)segment_ptr);
}

bool newrelic_validate_segment_param(const char* in, const char* name) {
  if (nr_strchr(in, '/')) {
    if (NULL == name) {
      nrl_error(NRL_INSTRUMENT, "parameter cannot include a slash");
    } else {
      nrl_error(NRL_INSTRUMENT, "%s cannot include a slash", name);
    }
    return false;
  }

  return true;
}

newrelic_segment_t* newrelic_start_segment(newrelic_txn_t* transaction,
                                           const char* name,
                                           const char* category) {
  newrelic_segment_t* segment = NULL;

  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT, "unable to start segment with NULL transaction");
    return NULL;
  }

  if (!name || !newrelic_validate_segment_param(name, "segment name")) {
    name = "Unnamed Segment";
  }

  if (!category
      || !newrelic_validate_segment_param(category, "segment category")) {
    category = "Custom";
  }

  /* Set up the fields so that we can correctly track child segment duration. */
  nrt_mutex_lock(&transaction->lock);
  {
    char* metric_name;

    /* Start the segment. */
    segment = newrelic_segment_create(transaction->txn);
    if (NULL == segment) {
      goto unlock_and_end;
    }

    /* Set the segment name. */
    metric_name = nr_formatf("%s/%s", category, name);
    nr_segment_set_name(segment->segment, metric_name);
    nr_free(metric_name);

  unlock_and_end:;
  }
  nrt_mutex_unlock(&transaction->lock);

  segment->txn_lock = &transaction->lock;

  return segment;
}

bool newrelic_set_segment_parent(newrelic_segment_t* segment,
                                 newrelic_segment_t* parent) {
  bool ret;

  if (NULL == segment) {
    nrl_error(NRL_INSTRUMENT, "unable to set the parent on a NULL segment");
    return false;
  }

  if (NULL == parent) {
    nrl_error(NRL_INSTRUMENT,
              "unable to set the parent of a segment to be a NULL segment");
    return false;
  }

  if (segment == parent) {
    nrl_error(NRL_INSTRUMENT,
              "unable to set the parent of a segment to be itself");
    return false;
  }

  nrt_mutex_lock(segment->txn_lock);
  newrelic_add_api_supportability_metric(segment->transaction,
                                         "set_segment_parent");
  ret = nr_segment_set_parent(segment->segment, parent->segment);
  nrt_mutex_unlock(segment->txn_lock);

  return ret;
}

bool newrelic_set_segment_parent_root(newrelic_segment_t* segment) {
  bool ret;

  if (NULL == segment) {
    nrl_error(NRL_INSTRUMENT, "unable to set the parent on a NULL segment");
    return false;
  }

  if (NULL == segment->transaction) {
    nrl_error(NRL_INSTRUMENT,
              "unable to set the parent on a segment without a transaction");
    return false;
  }

  if (NULL == segment->transaction->segment_root) {
    nrl_error(
        NRL_INSTRUMENT,
        "unable to set the parent on a segment with a malformed transaction");
    return false;
  }

  nrt_mutex_lock(segment->txn_lock);
  {
    newrelic_add_api_supportability_metric(segment->transaction,
                                           "set_segment_parent_root");
    ret = nr_segment_set_parent(segment->segment,
                                segment->transaction->segment_root);
  }
  nrt_mutex_unlock(segment->txn_lock);

  return ret;
}

bool newrelic_set_segment_timing(newrelic_segment_t* segment,
                                 newrelic_time_us_t start_time,
                                 newrelic_time_us_t duration) {
  bool ret;

  if (NULL == segment) {
    nrl_error(NRL_INSTRUMENT, "unable to set timing on a NULL segment");
    return false;
  }

  nrt_mutex_lock(segment->txn_lock);
  ret = nr_segment_set_timing(segment->segment, start_time, duration);
  nrt_mutex_unlock(segment->txn_lock);

  return ret;
}

bool newrelic_end_segment(newrelic_txn_t* transaction,
                          newrelic_segment_t** segment_ptr) {
  newrelic_segment_t* segment;
  bool status = false;

  if ((NULL == segment_ptr) || (NULL == *segment_ptr)) {
    nrl_error(NRL_INSTRUMENT, "unable to end a NULL segment");
    return false;
  }
  segment = *segment_ptr;

  if (NULL == segment->transaction || NULL == transaction) {
    nrl_error(NRL_INSTRUMENT, "unable to end a segment of a NULL transaction");
    goto end;
  }

  /* Sanity check that the segment is being ended on the same transaction it
   * was started on. Transitioning a segment between transactions would be
   * problematic, since times are transaction-specific.
   * */
  if (transaction->txn != segment->transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot end a segment on a different transaction to the one it "
              "was created on");
    goto end;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    switch (segment->segment->type) {
      case NR_SEGMENT_CUSTOM:
        /* Stop the segment. */
        nr_segment_end(segment->segment);

        /* Add a custom metric. */
        nr_segment_add_metric(segment->segment,
                              nr_string_get(transaction->txn->trace_strings,
                                            segment->segment->name),
                              true);
        break;

      case NR_SEGMENT_DATASTORE:
        newrelic_end_datastore_segment(segment);
        break;

      case NR_SEGMENT_EXTERNAL:
        newrelic_end_external_segment(segment);
        break;

      default:
        nrl_error(NRL_INSTRUMENT, "unknown segment type %d",
                  (int)segment->segment->type);
        status = false;
    }
  }
  nrt_mutex_unlock(&transaction->lock);

  status = true;

end:
  newrelic_segment_destroy(segment_ptr);

  return status;
}
