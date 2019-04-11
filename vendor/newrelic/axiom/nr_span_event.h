#ifndef NR_SPAN_EVENT_H
#define NR_SPAN_EVENT_H

#include "util_sampling.h"
#include "util_time.h"
#include "nr_distributed_trace.h"

/*
 * Purpose : The public version of the distributed trace structs/types
 */
typedef struct _nr_span_event_t nr_span_event_t;

typedef enum {
  NR_SPAN_GENERIC,
  NR_SPAN_HTTP,
  NR_SPAN_DATASTORE
} nr_span_category_t;

typedef enum {
  NR_SPAN_DATASTORE_COMPONENT,
  NR_SPAN_DATASTORE_DB_STATEMENT,
  NR_SPAN_DATASTORE_DB_INSTANCE,
  NR_SPAN_DATASTORE_PEER_ADDRESS,
  NR_SPAN_DATASTORE_PEER_HOSTNAME
} nr_span_event_datastore_member_t;

typedef enum {
  NR_SPAN_EXTERNAL_COMPONENT,
  NR_SPAN_EXTERNAL_URL,
  NR_SPAN_EXTERNAL_METHOD
} nr_span_event_exernal_member_t;

/*
 * Purpose : Creates/allocates a new span event metadata struct
 *           instance. It's the responsibility of the caller to
 *           free/destroy the struct with the nr_span_event_destroy
 *           function.
 *
 * Params  : distributed trace object - span events require some information
 *           from the distributed trace object.
 *           guid - the guid is passed into the constructor it cannot be changed
 *
 * Returns : An allocated nr_span_event_t that the caller owns and must
 *           destroy with nr_span_event_destroy().
 */
nr_span_event_t* nr_span_event_create(void);

/*
 * Purpose : Destroys/frees structs created via nr_span_event_create.
 *
 * Params  : A pointer to the pointer that points at the allocated
 *           nr_span_event_t (created with nr_span_event_create).
 *
 * Returns : Nothing.
 */
void nr_span_event_destroy(nr_span_event_t** ptr);

/*
 * Purpose : Get the various fields of the span events.
 *
 * Params : The span event.
 *
 * Returns : The stated field.
 */
extern const char* nr_span_event_get_guid(const nr_span_event_t* event);
extern const nr_span_event_t* nr_span_event_get_parent(
    const nr_span_event_t* event);
extern const char* nr_span_event_get_transaction_id(
    const nr_span_event_t* event);
extern const char* nr_span_event_get_name(const nr_span_event_t* event);
extern nr_span_category_t nr_span_event_get_category(
    const nr_span_event_t* event);
extern nrtime_t nr_span_event_get_timestamp(const nr_span_event_t* event);
extern nrtime_t nr_span_event_get_duration(const nr_span_event_t* event);
bool nr_span_event_is_entry_point(const nr_span_event_t* event);

/*
 * Purpose : Access a datastore attribute.
 *
 * Params : 1. The span event that you intend to access.
 *          2. The target field to be access, using the
 * nr_span_event_datastore_member_t enum.
 *
 * Returns : The value of the span field.
 */
const char* nr_span_event_get_datastore(
    const nr_span_event_t* event,
    nr_span_event_datastore_member_t member);

/*
 * Purpose : Access an external attribute.
 *
 * Params : 1. The span event you intend to access.
 *          2. The target field to be accessed.
 *
 * Returns : The value of the span field.
 */
extern const char* nr_span_event_get_external(
    const nr_span_event_t* event,
    nr_span_event_exernal_member_t member);
/*
 * Purpose : Set the various fields of the span events.
 *
 * Params : 1. The span event.
 *          2. The field to be set.
 *
 * Returns : Nothing.
 */
extern void nr_span_event_set_guid(nr_span_event_t* event, const char* guid);
extern void nr_span_event_set_parent(nr_span_event_t* event,
                                     const nr_span_event_t* parent_event);
extern void nr_span_event_set_transaction_id(nr_span_event_t* event,
                                             const char* transaction_id);
extern void nr_span_event_set_name(nr_span_event_t* event, const char* name);
extern void nr_span_event_set_category(nr_span_event_t* event,
                                       nr_span_category_t category);
extern void nr_span_event_set_timestamp(nr_span_event_t* event, nrtime_t time);
extern void nr_span_event_set_duration(nr_span_event_t* event,
                                       nrtime_t duration);

/*
 * Purpose : Set datastore fields.
 *
 * Params : 1. The Span Event that you would like changed
 *          2. The member of the struct you would like to update, using the
 *          nr_span_event_datastore_member_t enum.
 *          3. The string value would like stored in the specified field
 *
 * Returns : Nothing.
 */
extern void nr_span_event_set_datastore(nr_span_event_t* event,
                                        nr_span_event_datastore_member_t member,
                                        const char* new_value);

/*
 * Purpose : Set a external attributes.
 *
 * Params : 1. The target Span Event that should be changed.
 *          2. The external attribute to be set.
 *          3. The string value that the field will be after the function has
 * executed.
 */
extern void nr_span_event_set_external(nr_span_event_t* event,
                                       nr_span_event_exernal_member_t member,
                                       const char* new_value);

#endif /* NR_SPAN_EVENT_H */
