/*!
 * @file error.h
 *
 * @brief Function declarations necessary to support noticing errors.
 */
#ifndef LIBNEWRELIC_ERROR_H
#define LIBNEWRELIC_ERROR_H

#include "libnewrelic.h"

/**
 * @brief Record an error in a transaction, optionally passing a custom stacktrace
 *
 * Given an active transaction, this function records an error
 * inside of the transaction. A custom stacktrace can be passed as a string
 * in JSON format like: "[\"first line\",\"second line\",\"third line\"]".
 * Otherwise, if NULL is given, a stacktrace will be generated (if configured to do so).
 *
 * @param [in]  transaction An active transaction.
 * @param [in]  priority The error's priority. The C SDK sends up one error per
 *              transaction.  If multiple calls to this function are made during
 *              a single transaction, the error with the highest priority is
 *              reported to New Relic.
 * @param [in]  errmsg A string comprising the error message.
 * @param [in]  errclass A string comprising the error class.
 * @param [in]  errstacktrace A string comprising the error stacktrace, in NewRelic's JSON format, or NULL
 */
void newrelic_do_notice_error(newrelic_txn_t* transaction,
                           int priority,
                           const char* errmsg,
                           const char* errclass,
                           const char* errstacktrace);

#endif /* LIBNEWRELIC_ERROR_H */
