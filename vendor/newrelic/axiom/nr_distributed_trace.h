#ifndef NR_DISTRIBUTED_TRACE_HDR
#define NR_DISTRIBUTED_TRACE_HDR

#include <stdbool.h>

#include "util_sampling.h"
#include "util_time.h"
#include "util_object.h"

static const char NR_DISTRIBUTED_TRACE_ACCEPT_SUCCESS[]
    = "Supportability/DistributedTrace/AcceptPayload/Success";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_EXCEPTION[]
    = "Supportability/DistributedTrace/AcceptPayload/Exception";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_PARSE_EXCEPTION[]
    = "Supportability/DistributedTrace/AcceptPayload/ParseException";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_CREATE_BEFORE_ACCEPT[]
    = "Supportability/DistributedTrace/AcceptPayload/Ignored/"
      "CreateBeforeAccept";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_MULTIPLE[]
    = "Supportability/DistributedTrace/AcceptPayload/Ignored/Multiple";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_MAJOR_VERSION[]
    = "Supportability/DistributedTrace/AcceptPayload/Ignored/MajorVersion";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_NULL[]
    = "Supportability/DistributedTrace/AcceptPayload/Ignored/Null";
static const char NR_DISTRIBUTED_TRACE_ACCEPT_UNTRUSTED_ACCOUNT[]
    = "Supportability/DistributedTrace/AcceptPayload/Ignored/UntrustedAccount";
static const char NR_DISTRIBUTED_TRACE_CREATE_SUCCESS[]
    = "Supportability/DistributedTrace/CreatePayload/Success";
static const char NR_DISTRIBUTED_TRACE_CREATE_EXCEPTION[]
    = "Supportability/DistributedTrace/CreatePayload/Exception";
/*
 * Purpose : The public version of the distributed trace structs/types
 */
typedef struct _nr_distributed_trace_t nr_distributed_trace_t;
typedef struct _nr_distributed_trace_payload_t nr_distributed_trace_payload_t;

/*
 * Purpose : Creates/allocates a new distributed tracing metadata struct
 *           instance.  It's the responsibility of the caller to
 *           free/destroy the struct with the nr_distributed_trace_destroy
 *           function.
 *
 * Params  : None.
 *
 * Returns : An allocated nr_distributed_trace_t that the caller owns and must
             destroy with nr_distributed_trace_destroy().
 */
nr_distributed_trace_t* nr_distributed_trace_create(void);

/*
 * Purpose : Accepts an inbound distributed trace with an nrobj payload.
 *           The payload will copied to the inbound struct within
 *           the distributed trace.
 *
 * Params  : 1. A properly allocated distributed trace
 *           2. A nrobj containing the converted JSON payload
 *           3. The transport type of the payload, which has to be one of
 *              "Unknown", "HTTP", "HTTPS", "Kafka", "JMS", "IronMQ", "AMQP",
 *              "Queue" or "Other".
 *           4. An error string to be populated if an error occurs
 *
 * Returns : True on success, otherwise return false with a populated error
 *           string detailing the supportability metric name to report by the
 *           caller.
 */
bool nr_distributed_trace_accept_inbound_payload(nr_distributed_trace_t* dt,
                                                 const nrobj_t* obj_payload,
                                                 const char* transport_type,
                                                 const char** error);

/*
 * Purpose : Accepts a JSON payload, validates the payload and format, and
 *           returns an nrobj version of that payload.
 *
 * Params  : 1. A JSON payload
 *           2. An error string to be populated if an error occurs
 *
 * Returns : an nrobj on success, otherwise NULL with a populated error string
 *           detailing the supportability metric name to report by the caller.
 */
nrobj_t* nr_distributed_trace_convert_payload_to_object(const char* payload,
                                                        const char** error);

/*
 * Purpose : Destroys/frees structs created via nr_distributed_trace_create
 *
 * Params  : A pointer to the pointer that points at the allocated
 *           nr_distributed_trace_t (created with nr_distributed_trace_create)
 *
 * Returns : nothing
 */
void nr_distributed_trace_destroy(nr_distributed_trace_t** ptr);

/*
 * Purpose : Get the various fields of the distributed trace metadata
 *
 * Params  : 1. The distributed trace.
 *
 * Returns : The stated field.
 */
extern const char* nr_distributed_trace_get_account_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_get_app_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_get_guid(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_get_txn_id(
    const nr_distributed_trace_t* dt);
extern nr_sampling_priority_t nr_distributed_trace_get_priority(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_get_trace_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_get_trusted_key(
    const nr_distributed_trace_t* dt);
bool nr_distributed_trace_is_sampled(const nr_distributed_trace_t* dt);
bool nr_distributed_trace_inbound_is_set(const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_inbound_get_account_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_inbound_get_app_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_inbound_get_guid(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_inbound_get_txn_id(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_inbound_get_type(
    const nr_distributed_trace_t* dt);
extern nrtime_t nr_distributed_trace_inbound_get_timestamp_delta(
    const nr_distributed_trace_t* dt,
    nrtime_t txn_start);
extern const char* nr_distributed_trace_inbound_get_transport_type(
    const nr_distributed_trace_t* dt);
extern const char* nr_distributed_trace_object_get_account_id(
    const nrobj_t* object);
extern const char* nr_distributed_trace_object_get_trusted_key(
    const nrobj_t* object);

/*
 * Purpose : Set the distributed trace GUID (the current span identifier).
 *
 * Params  : 1. The distributed trace.
 *           2. The GUID (current span identifier)..
 */
extern void nr_distributed_trace_set_guid(nr_distributed_trace_t* dt,
                                          const char* guid);

/*
 * Purpose : Set the transaction id.
 *
 * Params  : 1. The distributed trace.
 *           2. The transaction id.
 */
extern void nr_distributed_trace_set_txn_id(nr_distributed_trace_t* dt,
                                            const char* guid);

/*
 * Purpose : Set the Account ID.
 *
 * Params  : 1. The distributed trace.
 *           2. The Account ID.
 */
extern void nr_distributed_trace_set_account_id(nr_distributed_trace_t* dt,
                                                const char* account_id);

/*
 * Purpose : Set the distributed trace App ID.
 *
 * Params  : 1. The distributed trace.
 *           2. The App ID.
 */
extern void nr_distributed_trace_set_app_id(nr_distributed_trace_t* dt,
                                            const char* app_id);

/*
 * Purpose : Set the distributed trace trace id.
 *
 * Params  : 1. The distributed trace.
 *           2. The trace id.
 */
void nr_distributed_trace_set_trace_id(nr_distributed_trace_t* dt,
                                       const char* trace_id);

/*
 * Purpose : Set the distributed trace priority.
 *
 * Params  : 1. The distributed trace.
 *           2. The sampling priority.
 */
extern void nr_distributed_trace_set_priority(nr_distributed_trace_t* dt,
                                              nr_sampling_priority_t priority);

/*
 * Purpose : Set the trusted account key.
 *
 * Params  : 1. The distributed trace.
 *           2. The trusted account key..
 */
extern void nr_distributed_trace_set_trusted_key(nr_distributed_trace_t* dt,
                                                 const char* trusted_key);
/*
 * Purpose :  Setter function for the sampled property
 *
 * Params  :  1. The nr_distributed_trace_t* whose property we want to set
 *            2. The bool value to set for the sampled property
 */
void nr_distributed_trace_set_sampled(nr_distributed_trace_t* dt, bool value);

/*
 * Purpose :  Setter function for the transport type
 *
 * Params  :  1. the nr_distributed_trace_t* whose property we want to set
 *            2. the string to set for the transport type
 */
void nr_distributed_trace_inbound_set_transport_type(nr_distributed_trace_t* dt,
                                                     const char* value);

/*
 * Purpose :  Create/allocates a new distributed tracing payload instance,
 *
 * Params  :  1. A pointer to the distributed trace metadata for this payload.
 *            2. The initial value for parent_id.
 *
 * Returns :  An allocated nr_distributed_trace_t that the caller
 *            must destroy with nr_distributed_trace_payload_destroy
 */
nr_distributed_trace_payload_t* nr_distributed_trace_payload_create(
    nr_distributed_trace_t* metadata,
    const char* parent_id);

/*
 * Purpose : Destroys/frees structs created via
 *           nr_distributed_trace_payload_create
 *
 * Params  :  A pointer to the pointer that points at the allocated
 *            nr_distributed_trace_payload_t (created with
 *            nr_distributed_trace_payload_create)
 *
 * Returns : nothing
 */
void nr_distributed_trace_payload_destroy(nr_distributed_trace_payload_t** ptr);

/*
 * Purpose : Get the various fields of the distributed trace payload
 *
 * Params  : The distributed trace payload.
 *
 * Returns : The stated field.
 */
const char* nr_distributed_trace_payload_get_parent_id(
    const nr_distributed_trace_payload_t* payload);
nrtime_t nr_distributed_trace_payload_get_timestamp(
    const nr_distributed_trace_payload_t* payload);
const nr_distributed_trace_t* nr_distributed_trace_payload_get_metadata(
    const nr_distributed_trace_payload_t* payload);

/*
 * Purpose : Create the text representation of the distributed trace payload.
 *
 * Params  : 1. The distributed trace payload.
 *
 * Returns : A newly allocated, null terminated payload string, which the caller
 *           must destroy with nr_free() when no longer needed, or NULL on
 *           error.
 */
extern char* nr_distributed_trace_payload_as_text(
    const nr_distributed_trace_payload_t* payload);

#endif /* NR_DISTRIBUTED_TRACE_HDR */
