/*!
 * @file transaction.h
 *
 * @brief Function declarations necessary to support starting transactions
 * in the C agent.
 */
#ifndef LIBNEWRELIC_TRANSACTION_H
#define LIBNEWRELIC_TRANSACTION_H

#include "nr_txn.h"
#include "util_threads.h"

/*!
 * @brief The internal transaction type.
 */
typedef struct _newrelic_txn_t {
  /*! The axiom transaction. */
  nrtxn_t* txn;

  /*! The transaction lock. */
  nrthread_mutex_t lock;
} newrelic_txn_t;

/*!
 * @brief Start a transaction
 *
 * @param [in] app An application.
 * @param [in] name The name of the transaction; may be NULL.
 * @param [in] is_web_transaction true if the transaction is a web transaction;
 * false otherwise.
 *
 * @return A pointer to an active transaction; NULL if the transaction could not
 * be started.
 */
newrelic_txn_t* newrelic_start_transaction(newrelic_app_t* app,
                                           const char* name,
                                           bool is_web_transaction);

#endif /* LIBNEWRELIC_TRANSACTION_H */
