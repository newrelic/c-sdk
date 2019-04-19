#ifndef NR_SPAN_EVENT_PRIVATE_H
#define NR_SPAN_EVENT_PRIVATE_H

#include "util_sampling.h"
#include "util_time.h"
#include "nr_span_event.h"

/*
 * Span Events
 *
 * These fields will not be accessed directly -- instead use the functions
 * in nr_span_event.h
 */

struct _nr_span_event_t {
  char* guid; /* The segment identifier */
  const nr_span_event_t*
      parent; /* The span event's parent (may be omitted for the root span) */
  char* transaction_id; /* the transaction's guid */
  nrtime_t
      timestamp; /* Unix timestamp in milliseconds when this segment started */
  nrtime_t duration;   /* Elapsed time in seconds */
  char* name;          /* Segment name */
  bool is_entry_point; /* This is always true (in the payload, otherwise it is
                          omitted) and only applied to the first segment */
  nr_span_category_t type;

  /*
   * The union type can only hold one struct at a time. This insures that we
   * will not reserve memory for variables that are not applicable for this type
   * of node. Example: A datastore node will not need to store a method and an
   * external node will not need to store a component.
   *
   * You must check the nr_span_category_t to determine which struct is being
   * used.
   */
  union {
    struct {
      char* component; /* The name of the database vendor or driver */
      char*
          db_statement;  /* The database statement in the format most permissive
                  by configuration */
      char* db_instance; /* The database name */
      char* peer_address;  /* A string formed from the host and portPathOrId:
                        "{host}:{portPathOrId}" */
      char* peer_hostname; /* The hostname of the database */
    } datastore;

    struct {
      char* component; /* The name of the framework being used to make the
                    connection */
      char* url; /* The external URI for the call. This MUST NOT contain user,
              password, or query parameters. */
      char*
          method; /* The HTTP method or language method / function used for the
                   * call
                   */
    } external;
  };
};

#endif /* NR_SPAN_EVENT_PRIVATE_H */
