/*
 * This file declares supporting functions for the distributed trace API.
 */
#ifndef PHP_API_DISTRIBUTED_TRACE_HDR
#define PHP_API_DISTRIBUTED_TRACE_HDR

/**
 * Purpose: Call for all PHP API distributed trace functions to call into
 *          axiom API(s) for accepting a base64 encoded distributed trace
 *          payload.
 *
 * Params:  1. The current transaction object
 *          2. The payload string in JSON (base64 encoded).
 *          3. The transport type
 *
 * Returns: True/False depending on the success or failure of underlying library
 */
extern bool nr_php_api_accept_distributed_trace_payload_httpsafe(
    nrtxn_t* txn,
    char* str_payload,
    char* transport_type);

/**
 * Purpose: Call for all PHP API distributed trace functions to call into
 *          axiom API(s) for accepting an already encoded  distributed trace
 *          payload.
 *
 * Params:  1. The current transaction object
 *          2. The payload string in JSON format (already decoded if need be)
 *          3. The transport type
 *
 * Returns: True/False depending on the success or failure of underlying library
 */
extern bool nr_php_api_accept_distributed_trace_payload(nrtxn_t* txn,
                                                        char* str_payload,
                                                        char* transport_type);

void nr_php_api_distributed_trace_register_userland_class(TSRMLS_D);

#endif /* PHP_API_DATASTORE_HDR */
