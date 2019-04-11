/*!
 * @file config.h
 *
 * @brief Type definitions, constants, and function declarations necessary to
 * support the C Agent's configuration.
 */
#ifndef LIBNEWRELIC_CONFIG_H
#define LIBNEWRELIC_CONFIG_H

#include "libnewrelic.h"
#include "nr_txn.h"

/*!
 * @brief Given a newrelic_tt_recordsql_t value return the
 * corresponding nr_tt_recordsql_t value.
 *
 * @return  If the string is NULL or any other value, return
 * the default NR_SQL_OBFUSCATED. Otherwise:
 *  - If NEWRELIC_SQL_OFF, return NR_SQL_NONE.
 *  - If NEWRELIC_SQL_RAW, return NR_SQL_RAW.
 *  - If NEWRELIC_SQL_OBFUSCATED, return NR_SQL_OBFUSCATED.
 */
nr_tt_recordsql_t newrelic_validate_recordsql(newrelic_tt_recordsql_t setting);

/*!
 * @brief Create a set of default agent configuration options
 *
 * @return A newly allocated nrtxnopt_t struct, which must be released
 * 				 with nr_free() when no longer required.
 */
nrtxnopt_t* newrelic_get_default_options(void);

/*!
 * @brief Convert a C agent configuration into transaction options.
 *
 * @param [in] config The configuration to convert. If NULL, default options
 *                    will be returned.
 *
 * @return A newly allocated nrtxnopt_t struct, which must be released with
 * nr_free() when no longer required.
 */
nrtxnopt_t* newrelic_get_transaction_options(
    const newrelic_app_config_t* config);

#endif /* LIBNEWRELIC_CONFIG_H */
