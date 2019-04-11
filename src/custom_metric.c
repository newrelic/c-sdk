#include <stdio.h>
#include "libnewrelic.h"
#include "nr_txn.h"
#include "transaction.h"

bool newrelic_record_custom_metric(newrelic_txn_t* transaction,
                                   const char* metric_name,
                                   double milliseconds) {
  nr_status_t ret;

  if (NULL == transaction || NULL == metric_name) {
    return false;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    ret = nr_txn_add_custom_metric(transaction->txn, metric_name, milliseconds);
  }
  nrt_mutex_unlock(&transaction->lock);

  return (NR_SUCCESS == ret);
}
