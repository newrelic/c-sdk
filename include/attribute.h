/*!
 * @file attribute.h
 *
 * @brief Type definitions, constants, and function declarations necessary to
 * support adding custom attributes to transactions in the C Agent.
 */
#ifndef LIBNEWRELIC_ATTRIBUTE_H
#define LIBNEWRELIC_ATTRIBUTE_H

#include "nr_txn.h"
#include "util_object.h"

/*!
 * @brief Add a custom attribute to a transaction.
 *
 * @param [in] transaction An active transaction.
 * @param [in] key The name of the attribute.
 * @param [in] obj An nrobj_t hash with the value of the attribute.
 *
 * @return true if successful; false otherwise.
 */
bool newrelic_add_attribute(newrelic_txn_t* transaction,
                            const char* key,
                            nrobj_t* obj);

#endif /* LIBNEWRELIC_ATTRIBUTE_H */
