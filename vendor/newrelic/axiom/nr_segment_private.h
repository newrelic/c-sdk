#ifndef NR_SEGMENT_PRIVATE_HDR
#define NR_SEGMENT_PRIVATE_HDR

#include "nr_segment.h"

/*
 * Purpose : Initialize a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *
 */
void nr_segment_children_init(nr_segment_children_t* children);

/*
 * Purpose : Get the sibling previous to or next to the given child.
 *           Also known as the pair of queries:
 *             "Who is your big sister?"
 *             "Who is your little brother?"
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. A pointer to the child of interest.
 *
 * Returns : A pointer to the sibling segment; NULL if no such sibling exists.
 *
 * Note    : At the time of this writing, this pair of functions are provided
 *           for internal testing purposes.
 */
nr_segment_t* nr_segment_children_get_prev(nr_segment_children_t* children,
                                           nr_segment_t* child);
nr_segment_t* nr_segment_children_get_next(nr_segment_children_t* children,
                                           nr_segment_t* child);

/*
 * Purpose : Add a child to a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. A pointer to the segment to add as a sibling.
 *
 * Warning : As this is an internal function, the children parameter is NOT
 *           checked for validity within this function. If it is possible that
 *           children may be NULL, you must check that before invoking this
 *           function.
 */
void nr_segment_children_add(nr_segment_children_t* children,
                             nr_segment_t* child);

/*
 * Purpose : Remove a child from a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. A pointer to the segment to remove.
 *
 * Returns : True if successful, false otherwise.
 */
bool nr_segment_children_remove(nr_segment_children_t* children,
                                nr_segment_t* child);

/*
 * Purpose : Free all data related to a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 */
void nr_segment_children_destroy_fields(nr_segment_children_t* children);

/*
 * Purpose : Free all data related to a segment's typed attributes.
 *
 * Params  : 1. A segment's type.
 *           2. A pointer to a segment's _nr_segment_typed_attributes_t
 *              structure.
 */
void nr_segment_destroy_typed_attributes(
    nr_segment_type_t type,
    union _nr_segment_typed_attributes_t* attributes);

/*
 * Purpose : Free all data related to a segment's datastore metadata.
 *
 * Params  : 1. A pointer to a segment's nr_segment_datastore_t structure.
 */
void nr_segment_datastore_destroy_fields(nr_segment_datastore_t* datastore);

/*
 * Purpose : Free all data related to a segment's external metadata.
 *
 * Params  : 1. A pointer to a segment's nr_segment_external_t structure.
 */
void nr_segment_external_destroy_fields(nr_segment_external_t* external);

/*
 * Purpose : Free all data related to a segment metric.
 *
 * Params  : 1. A pointer to a segment's nr_segment_metric_t structure.
 */
void nr_segment_metric_destroy_fields(nr_segment_metric_t* sm);

#endif
