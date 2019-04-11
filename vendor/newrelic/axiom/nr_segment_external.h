#ifndef NR_SEGMENT_EXTERNAL_HDR
#define NR_SEGMENT_EXTERNAL_HDR

#include "nr_segment.h"

typedef struct {
  char* library;                 /* The null-terminated library; if unset, this
                                    is ignored. */
  char* procedure;               /* The null-terminated procedure (or method);
                                    if unset, this is ignored. */
  char* uri;                     /* The URI. */
  bool do_rollup;                /* If true, adjacent nodes of the same type
                                    will be rolled up into a single node. */
  char* encoded_response_header; /* The encoded contents of the
                                    X-NewRelic-App-Data header. */
} nr_segment_external_params_t;

/*
 * Purpose : End an external segment and record metrics.
 *
 * Params  : 1. A pointer to the external segment to be ended.
 *           2. The parameters listed above.
 *
 * Returns: true on success.
 */
extern bool nr_segment_external_end(nr_segment_t* segment,
                                    const nr_segment_external_params_t* params);

#endif
