#include "libnewrelic.h"
#include "transaction.h"
#include "segment.h"

#include "nr_axiom.h"
#include "nr_distributed_trace.h"

#include "util_base64.h"
#include "util_logging.h"
#include "util_strings.h"

char* newrelic_create_distributed_trace_payload(newrelic_txn_t* transaction,
                                                newrelic_segment_t* segment) {
  char* payload;
  nr_segment_t* s = NULL;

  /* Affirm required function parameter is not NULL */
  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot invoke newrelic_create_distributed_trace_payload() on a "
              "NULL transaction");
    return NULL;
  }

  /* If no segment was supplied, then we must make a distributed
   * trace payload for the root segment on the main thread of execution */
  nrt_mutex_lock(&transaction->lock);
  if (NULL == segment) {
    s = transaction->txn->segment_root;
  } else {
    s = segment->segment;
  }

  payload = nr_txn_create_distributed_trace_payload(transaction->txn, s);
  nrt_mutex_unlock(&transaction->lock);

  return payload;
}

char* newrelic_create_distributed_trace_payload_httpsafe(
    newrelic_txn_t* transaction,
    newrelic_segment_t* segment) {
  int encoded_len = 0;
  char* encoded_payload;
  char* payload
      = newrelic_create_distributed_trace_payload(transaction, segment);

  if (!payload) {
    return NULL;
  }

  encoded_payload = nr_b64_encode(payload, nr_strlen(payload), &encoded_len);
  nr_free(payload);

  return encoded_payload;
}

bool newrelic_accept_distributed_trace_payload(newrelic_txn_t* transaction,
                                               const char* payload,
                                               const char* transport_type) {
  bool ret;

  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot invoke newrelic_accept_distributed_trace_payload() on a "
              "NULL transaction");
    return false;
  }

  nrt_mutex_lock(&transaction->lock);
  ret = nr_txn_accept_distributed_trace_payload(transaction->txn, payload,
                                                transport_type);
  nrt_mutex_unlock(&transaction->lock);

  return ret;
}

bool newrelic_accept_distributed_trace_payload_httpsafe(
    newrelic_txn_t* transaction,
    const char* payload,
    const char* transport_type) {
  bool ret;

  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT,
              "cannot invoke "
              "newrelic_accept_distributed_trace_payload_httpsafe() on a "
              "NULL transaction");
    return false;
  }

  nrt_mutex_lock(&transaction->lock);
  ret = nr_txn_accept_distributed_trace_payload_httpsafe(
      transaction->txn, payload, transport_type);
  nrt_mutex_unlock(&transaction->lock);

  return ret;
}
