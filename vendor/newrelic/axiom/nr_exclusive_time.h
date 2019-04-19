/*
 * This file provides a struct and associated functions to calculate exclusive
 * time for a segment.
 */
#ifndef NR_EXCLUSIVE_TIME_HDR
#define NR_EXCLUSIVE_TIME_HDR

#include <stdbool.h>

#include "util_time.h"
#include "util_vector.h"

typedef struct _nr_exclusive_time_t nr_exclusive_time_t;

/*
 * Purpose : Create an exclusive time structure.
 *
 * Params  : 1. The start time of the parent segment.
 *           2. The stop time of the parent segment.
 *
 * Returns : A pointer to the exclusive time structure, or NULL on error.
 */
extern nr_exclusive_time_t* nr_exclusive_time_create(nrtime_t start_time,
                                                     nrtime_t stop_time);

/*
 * Purpose : Destroy an exclusive time structure.
 *
 * Params  : 1. A pointer to the exclusive time structure.
 *
 * Returns : True on success; false otherwise.
 */
extern bool nr_exclusive_time_destroy(nr_exclusive_time_t** et_ptr);

/*
 * Purpose : Add a child period to the exclusive time structure.
 *
 *           The period described by the start and stop times will be removed
 *           from the exclusive time calculated for the parent segment.
 *
 * Params  : 1. A pointer to the exclusive time structure.
 *           2. The start time of the child segment.
 *           3. The stop time of the child segment.
 *
 * Returns : True on success; false otherwise.
 */
extern bool nr_exclusive_time_add_child(nr_exclusive_time_t* parent_et,
                                        nrtime_t start_time,
                                        nrtime_t stop_time);

/*
 * Purpose : Calculate how much exclusive time the parent segment actually had.
 *
 * Params  : 1. A pointer to the exclusive time structure.
 *
 * Returns : The amount of exclusive time. On error, 0 is returned, but note
 *           that 0 may also be a valid value.
 */
extern nrtime_t nr_exclusive_time_calculate(nr_exclusive_time_t* et);

#endif /* NR_EXCLUSIVE_TIME_HDR */
