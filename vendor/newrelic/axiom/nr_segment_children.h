/*
 * The segment children API.
 *
 * Most functions in this API are declared static inline within the header so
 * that the compiler may aggressively inline.
 */
#ifndef NR_SEGMENT_CHILDREN_HDR
#define NR_SEGMENT_CHILDREN_HDR

// This value must be below 2^31-1. If it's not, you have bigger problems.
#define NR_SEGMENT_CHILDREN_PACKED_LIMIT 8

#include <stdbool.h>

#include "util_memory.h"
#include "util_vector.h"

// Forward declaration of nr_segment_t, since we have a circular dependency with
// nr_segment.h.
typedef struct _nr_segment_t nr_segment_t;

/*
 * The shared header between the vector and packed versions of
 * nr_segment_children_t below.
 */
typedef struct _nr_segment_children_header_t {
  // The first field in nr_vector_t is capacity. We're going to repurpose the
  // highest bit of the capacity field to indicate whether the children
  // structure is packed or not.
  //
  // The observant will note that this does imply that the capacity of the
  // children vector may not be greater than either 2^31-1 or 2^63-1, depending
  // on the architecture. That's probably OK, but we could harden this by adding
  // an actual check for that.
  bool is_packed : 1;
#if UINTPTR_MAX == 0xffffffffffffffff
  size_t count : 63;
#elif UINTPTR_MAX == 0xffffffff
  size_t count : 31;
#else
#error Unknown pointer size.
#endif
} nr_segment_children_header_t;

/*
 * The actual children structure, which is actually a union, for added
 * confusion.
 */
typedef union _nr_segment_children_t {
  // Whether vector or packed is in use, the header is the same either way.
  // While it would be safe to access packed.header.is_packed no matter what,
  // this provides a slightly shorter and more obviously idiomatic way to access
  // the is_packed field.
  nr_segment_children_header_t header;

  nr_vector_t vector;

  struct {
    // Unused, but required to ensure the elements array doesn't overlap the
    // header.
    nr_segment_children_header_t header;
    nr_segment_t* elements[NR_SEGMENT_CHILDREN_PACKED_LIMIT];
  } packed;
} nr_segment_children_t;

#include "nr_segment_children_private.h"

/*
 * Purpose : Initialize a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 */
static inline void nr_segment_children_init(nr_segment_children_t* children) {
  if (nrunlikely(NULL == children)) {
    return;
  }

  children->header.is_packed = true;
  children->header.count = 0;
}

/*
 * Purpose : Deinitialise a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 */
static inline void nr_segment_children_deinit(nr_segment_children_t* children) {
  if (nrunlikely(NULL == children)) {
    return;
  }

  if (!children->header.is_packed) {
    nr_vector_deinit(&children->vector);
  }
  nr_segment_children_init(children);
}

/*
 * Purpose : Return the number of children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *
 * Returns : The number of children, or 0 on error.
 */
static inline size_t nr_segment_children_size(
    const nr_segment_children_t* children) {
  if (nrunlikely(NULL == children)) {
    return 0;
  }

  return children->header.is_packed ? children->header.count
                                    : nr_vector_size(&children->vector);
}

/*
 * Purpose : Return a child within a segment.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. The index of the child to return.
 *
 * Returns : The child element, or NULL on error.
 */
static inline nr_segment_t* nr_segment_children_get(
    nr_segment_children_t* children,
    size_t i) {
  // The nr_segment_children_size() call will also implicitly check for NULL.
  if (nrunlikely(i >= nr_segment_children_size(children))) {
    return NULL;
  }

  // This breaks the encapsulation of nr_vector_t, but avoids the checks that
  // we've already done above.
  return children->header.is_packed ? children->packed.elements[i]
                                    : children->vector.elements[i];
}

/*
 * Purpose : Add a child to a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. A pointer to the segment to add.
 *
 * Returns : True if successful, false otherwise.
 */
static inline bool nr_segment_children_add(nr_segment_children_t* children,
                                           nr_segment_t* child) {
  if (nrunlikely(NULL == children || NULL == child)) {
    return false;
  }

  if (children->header.is_packed) {
    size_t new_count = children->header.count + 1;

    if (new_count > NR_SEGMENT_CHILDREN_PACKED_LIMIT) {
      // We're about to overflow the packed array; migrate to a vector.
      nr_segment_children_migrate_to_vector(children);
      nr_segment_children_add_vector(children, child);
    } else {
      children->packed.elements[children->header.count] = child;
      children->header.count = new_count;
    }
  } else {
    nr_segment_children_add_vector(children, child);
  }

  return true;
}

/*
 * Purpose : Remove a child from a segment's children.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. A pointer to the segment to remove.
 *
 * Returns : True if successful, false otherwise.
 */
static inline bool nr_segment_children_remove(nr_segment_children_t* children,
                                              const nr_segment_t* child) {
  if (nrunlikely(NULL == children || NULL == child
                 || 0 == nr_segment_children_size(children))) {
    return false;
  }

  if (children->header.is_packed) {
    size_t i;
    const size_t end = children->header.count - 1;

    if (child == children->packed.elements[end]) {
      // The simple case: the child is the last element, so we can just
      // decrement the count and we're done.
      children->header.count -= 1;
      return true;
    }

    for (i = 0; i < end; i++) {
      if (child == children->packed.elements[i]) {
        nr_memmove(&children->packed.elements[i],
                   &children->packed.elements[i + 1],
                   sizeof(nr_segment_t*) * (children->header.count - i - 1));
        children->header.count -= 1;
        return true;
      }
    }
  } else {
    size_t index;

    if (nr_vector_find_first(&children->vector, child, NULL, NULL, &index)) {
      void* element;

      return nr_vector_remove(&children->vector, index, &element);
    }
  }

  return false;
}

/*
 * Purpose : Reparent all children onto a new parent.
 *
 * Params  : 1. A pointer to a segment's nr_segment_children_t structure.
 *           2. The new parent segment.
 *
 * Returns : True on success; false otherwise.
 */
extern bool nr_segment_children_reparent(nr_segment_children_t* children,
                                         nr_segment_t* new_parent);

#endif /* NR_SEGMENT_CHILDREN_HDR */
