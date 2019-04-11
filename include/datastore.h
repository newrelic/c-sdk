/*!
 * @file datastore.h
 *
 * @brief Type definitions, constants, and function declarations necessary to
 * support datastore segments, or axiom datastore nodes, for the C agent.
 */
#ifndef LIBNEWRELIC_DATASTORE_H
#define LIBNEWRELIC_DATASTORE_H

#include "segment.h"

/*!
 * @brief Destroy the datastore-specific fields in a segment.
 *
 * @param [in] segment The segment being destroyed.
 */
extern void newrelic_destroy_datastore_segment_fields(
    newrelic_segment_t* segment);

/*!
 * @brief End a datastore segment.
 *
 * This function assumes that the transaction has already been locked.
 *
 * @param [in] segment The segment that is ending.
 * @return True if the datastore metrics were sent; false otherwise.
 */
extern bool newrelic_end_datastore_segment(newrelic_segment_t* segment);

#endif /* LIBNEWRELIC_DATASTORE_H */
