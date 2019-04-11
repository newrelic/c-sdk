#ifndef UTIL_VECTOR_PRIVATE_HDR
#define UTIL_VECTOR_PRIVATE_HDR

/*
 * Purpose : Shrink a vector if necessary.
 *
 * Params  : 1. The vector.
 *
 * Returns : True if the vector was shrunk or did not need to be shrunk; false
 *           if shrinkage failed.
 */
extern bool nr_vector_shrink_if_necessary(nr_vector_t* v);

#endif /* UTIL_VECTOR_PRIVATE_HDR */
