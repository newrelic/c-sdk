#include "libnewrelic.h"
#include "attribute.h"
#include "transaction.h"

#include "nr_axiom.h"
#include "nr_attributes.h"
#include "util_logging.h"

bool newrelic_add_attribute(newrelic_txn_t* transaction,
                            const char* key,
                            nrobj_t* obj) {
  bool success = false;

  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT, "unable to add attribute for a NULL transaction");
    return success;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    if (NULL == key) {
      nrl_error(NRL_INSTRUMENT, "unable to add attribute with a NULL key");
      goto end;
    }

    if (NULL == obj) {
      nrl_error(NRL_INSTRUMENT, "unable to add attribute with a NULL value");
      goto end;
    }

    if (NR_FAILURE
        == nr_txn_add_user_custom_parameter(transaction->txn, key, obj)) {
      nrl_error(NRL_INSTRUMENT, "unable to add attribute for key=\"%s\"", key);
      goto end;
    }

    success = true;
  }
end:
  nrt_mutex_unlock(&transaction->lock);

  return success;
}

bool newrelic_add_attribute_int(newrelic_txn_t* transaction,
                                const char* key,
                                const int value) {
  nrobj_t* obj;
  bool outcome;

  obj = nro_new_int(value);
  outcome = newrelic_add_attribute(transaction, key, obj);
  nro_delete(obj);

  return outcome;
}

bool newrelic_add_attribute_long(newrelic_txn_t* transaction,
                                 const char* key,
                                 const long value) {
  nrobj_t* obj;
  bool outcome;

  obj = nro_new_long(value);
  outcome = newrelic_add_attribute(transaction, key, obj);
  nro_delete(obj);

  return outcome;
}

bool newrelic_add_attribute_double(newrelic_txn_t* transaction,
                                   const char* key,
                                   const double value) {
  nrobj_t* obj;
  bool outcome;

  obj = nro_new_double(value);
  outcome = newrelic_add_attribute(transaction, key, obj);
  nro_delete(obj);

  return outcome;
}

bool newrelic_add_attribute_string(newrelic_txn_t* transaction,
                                   const char* key,
                                   const char* value) {
  nrobj_t* obj;
  bool outcome;

  if (NULL == value) {
    nrl_error(NRL_INSTRUMENT, "unable to add attribute with a NULL value");
    return false;
  }

  obj = nro_new_string(value);
  outcome = newrelic_add_attribute(transaction, key, obj);
  nro_delete(obj);

  return outcome;
}
