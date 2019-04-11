/*!
 * @file global.h
 *
 * @brief Internal functions used to support global (per-process)
 * configuration, setup, and teardown of the C agent.
 */
#ifndef LIBNEWRELIC_GLOBAL_H
#define LIBNEWRELIC_GLOBAL_H
#include "nr_txn.h"

/*!
 * @brief Actually initialise the C agent.
 *
 * @param [in] daemon_socket The path to the daemon socket.
 * @param [in] time_limit_ms The time, in milliseconds, to wait for the daemon
 * connection to be established.
 * @return true on success; false otherwise.
 */
bool newrelic_do_init(const char* daemon_socket, int time_limit_ms);

/*!
 * @brief Ensure that the C agent has been initialised.
 *
 * If the C agent has not previously been initialised, this is equivalent to a
 * call to newrelic_init(NULL, 0).
 *
 * @return true on success; false otherwise.
 */
bool newrelic_ensure_init(void);

/*!
 * @brief Shut down the C agent.
 *
 * In the normal course of events, this is handled by an atexit() handler
 * installed by newrelic_do_init().
 */
void newrelic_shutdown(void);

/*!
 * Whether the log system has been initialised.
 */
extern bool newrelic_log_configured;

/*!
 * @brief Add api supportability metrics to the transaction.
 *
 * @param name The name of the api function, this will be added to the end of
 * the metric.
 * @param txn The Transaction that the metric will be attached to.
 */
void newrelic_add_api_supportability_metric(nrtxn_t* txn, const char* name);

#endif /* LIBNEWRELIC_GLOBAL_H */
