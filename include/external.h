/*!
 * @file external.h
 *
 * @brief Type definitions, constants, and function declarations necessary to
 * support external segments in the C Agent.
 */
#ifndef LIBNEWRELIC_EXTERNAL_H
#define LIBNEWRELIC_EXTERNAL_H

#include "segment.h"

/*!
 * @brief Destroy the external-specific fields in a segment.
 *
 * @param [in] segment The segment being destroyed.
 */
void newrelic_destroy_external_segment_fields(newrelic_segment_t* segment);

/*!
 * @brief End an external segment.
 *
 * This function assumes that the transaction has already been locked.
 *
 * @param [in] segment The segment that is ending.
 * @return True if the external metrics were sent; false otherwise.
 */
extern bool newrelic_end_external_segment(newrelic_segment_t* segment);

#endif /* LIBNEWRELIC_EXTERNAL_H */
