#ifndef UTIL_SLAB_PRIVATE_HDR
#define UTIL_SLAB_PRIVATE_HDR

/*
 * A page within the slab allocator.
 *
 * Note that these are allocated as variably sized structures via mmap(), and
 * the data is stored in the array packed in via the data element.
 */
typedef struct _nr_slab_page_t {
  struct _nr_slab_page_t* prev;

  // The capacity and amount used are stored in bytes, not objects. (If we ever
  // decide to extend this to support heterogeneous object allocation, that'll
  // be handy.)
  size_t capacity;
  size_t used;

  char data[0];
} nr_slab_page_t;

/*
 * The actual slab allocator.
 *
 * In practice, objects are allocated from "pages" within the slab allocator
 * (which may or may not be multiple pages at the OS level). These pages are
 * maintained as a single linked list, since we only ever need the current one
 * until destruction, at which point we can just iterate the list.
 */
struct _nr_slab_t {
  nr_slab_page_t* head;
  size_t object_size;
  size_t page_size;
};

#endif /* UTIL_SLAB_PRIVATE_HDR */
