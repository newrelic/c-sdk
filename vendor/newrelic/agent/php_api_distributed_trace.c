#include "php_agent.h"

#include "php_api_distributed_trace.h"
#include "php_call.h"
#include "php_hash.h"
#include "util_base64.h"
#include "util_logging.h"

#include "nr_distributed_trace.h"

/* {{{ newrelic\DistributedTracePayload class definition and methods */

/*
 * True global for the DistributedTracePayload class entry.
 */
zend_class_entry* nr_distributed_trace_payload_ce;

static const char payload_text_prop[] = "text";

/*
 * Arginfo for the DistributedTracePayload methods
 */
ZEND_BEGIN_ARG_INFO_EX(nr_distributed_trace_payload_text_arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(nr_distributed_trace_payload_httpsafe_arginfo_void,
                       0,
                       0,
                       0)
ZEND_END_ARG_INFO()

/*
 * DistributedTracePayload methods
 */
static PHP_NAMED_FUNCTION(nr_distributed_trace_payload_httpsafe) {
  char* encoded;
  int encoded_len = 0;
  zval* text;

  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;

  if (FAILURE == zend_parse_parameters_none()) {
    RETURN_FALSE;
  }

  text = nr_php_get_zval_object_property(NR_PHP_INTERNAL_FN_THIS,
                                         payload_text_prop TSRMLS_CC);

  /* nr_b64_encode() will return false if given an empty string, so we'll early
   * return here in that case. */
  if (0 == Z_STRLEN_P(text)) {
    nr_php_zval_str(return_value, "");
    return;
  }

  encoded = nr_b64_encode(Z_STRVAL_P(text), NRSAFELEN(Z_STRLEN_P(text)),
                          &encoded_len);

  if (NULL == encoded) {
    zend_error(E_WARNING,
               "Error encoding text payload to the HTTP safe format");
    nr_php_zval_str(return_value, "");
    return;
  }

  nr_php_zval_str_len(return_value, encoded, encoded_len);
  nr_free(encoded);
}

static PHP_NAMED_FUNCTION(nr_distributed_trace_payload_text) {
  zval* text;

  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;

  if (FAILURE == zend_parse_parameters_none()) {
    RETURN_FALSE;
  }

  text = nr_php_get_zval_object_property(NR_PHP_INTERNAL_FN_THIS,
                                         payload_text_prop TSRMLS_CC);
  RETURN_ZVAL(text, 1, 0);
}

/*
 * The method array for the DistributedTracePayload class.
 */

// clang-format off
const zend_function_entry nr_distributed_trace_payload_functions[]
    = {ZEND_FENTRY(text,
                   nr_distributed_trace_payload_text,
                   nr_distributed_trace_payload_text_arginfo_void,
                   ZEND_ACC_PUBLIC)
       ZEND_FENTRY(httpSafe,
                   nr_distributed_trace_payload_httpsafe,
                   nr_distributed_trace_payload_httpsafe_arginfo_void,
                   ZEND_ACC_PUBLIC)
       PHP_FE_END};
// clang-format on

/* }}} */

void nr_php_api_distributed_trace_register_userland_class(TSRMLS_D) {
  zend_class_entry tmp_nr_distributed_trace_payload_ce;
  INIT_CLASS_ENTRY(tmp_nr_distributed_trace_payload_ce,
                   "newrelic\\DistributedTracePayload",
                   nr_distributed_trace_payload_functions);

  nr_distributed_trace_payload_ce = zend_register_internal_class(
      &tmp_nr_distributed_trace_payload_ce TSRMLS_CC);

  /* We'll use a true property to store the text to avoid having to abstract the
   * significant differences in how object stores work between PHP 5 and 7. */
  zend_declare_property_string(
      nr_distributed_trace_payload_ce, nr_remove_const(payload_text_prop),
      sizeof(payload_text_prop) - 1, "", ZEND_ACC_PRIVATE TSRMLS_CC);
}

/*
 * New Relic API: Create a payload for instrumenting an outbound request with
 * Distributed Trace support.
 *    newrelic_create_distributed_trace_payload()
 */
#ifdef TAGS
void zif_newrelic_create_distributed_trace_payload(
    void); /* ctags landing pad only */
void newrelic_create_distributed_trace_payload(
    void); /* ctags landing pad only */
#endif
PHP_FUNCTION(newrelic_create_distributed_trace_payload) {
  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;
  NR_UNUSED_THIS_PTR;

  if (FAILURE == zend_parse_parameters_none()) {
    nrl_warning(
        NRL_API,
        "unable to parse parameters to "
        "newrelic_create_distributed_trace_payload; %d parameters received",
        ZEND_NUM_ARGS());
    RETURN_FALSE;
  }

  /* With the exception of parameter parsing errors (handled above), we're
   * always going to return a valid object so that the user can unconditionally
   * invoke methods on it. Let's do that now. */
  object_init_ex(return_value, nr_distributed_trace_payload_ce);

  /* Now we check if we're recording a transaction. */
  if (nr_php_recording(TSRMLS_C)) {
    /* nr_txn_create_distributed_trace_payload() will return NULL if distributed
     * tracing is not enabled, so we don't need to handle that explicitly here.
     */
    char* payload = nr_txn_create_distributed_trace_payload(NRPRG(txn));

    if (payload) {
      zend_update_property_string(nr_distributed_trace_payload_ce, return_value,
                                  nr_remove_const(payload_text_prop),
                                  sizeof(payload_text_prop) - 1,
                                  payload TSRMLS_CC);
      nr_free(payload);
    }
  }
}

bool nr_php_api_accept_distributed_trace_payload(nrtxn_t* txn,
                                                 char* str_payload,
                                                 char* transport_type) {
  if (NULL == txn) {
    return false;
  }

  return nr_txn_accept_distributed_trace_payload(txn, str_payload,
                                                 transport_type);
}

bool nr_php_api_accept_distributed_trace_payload_httpsafe(
    nrtxn_t* txn,
    char* str_payload,
    char* transport_type) {
  if (NULL == txn) {
    return false;
  }

  return nr_txn_accept_distributed_trace_payload_httpsafe(txn, str_payload,
                                                          transport_type);
}

/*
 * New Relic API: Accept A Distributed Trace Payload as a JSON encoded string
 *                and hands it off to axiom for further processing
 *
 * Params       : 1. A string containing an HTTPSafe (Base64)
 *                   JSON encoded payload
 *                2. An optional string allowing the user override the
 *                   transport type
 *
 * Returns:     Boolean for success or failure
 */
#ifdef TAGS
void zif_newrelic_accept_distributed_trace_payload_httpsafe(
    void); /* ctags landing pad only */
void newrelic_accept_distributed_trace_payload_httpsafe(
    void); /* ctags landing pad only */
#endif
PHP_FUNCTION(newrelic_accept_distributed_trace_payload_httpsafe) {
  char* payload_arg = 0;
  char* payload_string = 0;
  char* type_arg = 0;
  char* type_string = 0;

  nr_string_len_t payload_arg_length = 0;
  nr_string_len_t type_arg_length = 0;

  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;
  NR_UNUSED_THIS_PTR;

  if (SUCCESS
      == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &payload_arg,
                               &payload_arg_length, &type_arg,
                               &type_arg_length)) {
    payload_string = (char*)nr_alloca(payload_arg_length + 1);
    nr_strxcpy(payload_string, payload_arg, payload_arg_length);

    if (type_arg) {
      type_string = (char*)nr_alloca(type_arg_length + 1);
      nr_strxcpy(type_string, type_arg, type_arg_length);
    }
  }

  // args are parsed, do the work, and then cleanup
  if (nr_php_api_accept_distributed_trace_payload_httpsafe(
          NRPRG(txn), payload_string, type_string)) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}

/*
 * New Relic API: Accept A Distributed Trace Payload as a JSON encoded string
 *                and hands it off to axiom for further processing
 *
 * Params       : 1. A string containing a JSON encoded payload
 *                2. An optional string allowing the user override the
 *                   transport type
 *
 * Returns:     Boolean for success or failure
 */
#ifdef TAGS
void zif_newrelic_accept_distributed_trace_payload(
    void); /* ctags landing pad only */
void newrelic_accept_distributed_trace_payload(
    void); /* ctags landing pad only */
#endif
PHP_FUNCTION(newrelic_accept_distributed_trace_payload) {
  char* payload_arg = 0;
  char* payload_string = 0;
  char* type_arg = 0;
  char* type_string = 0;

  nr_string_len_t payload_arg_length = 0;
  nr_string_len_t type_arg_length = 0;

  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;
  NR_UNUSED_THIS_PTR;

  if (SUCCESS
      == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &payload_arg,
                               &payload_arg_length, &type_arg,
                               &type_arg_length)) {
    payload_string = (char*)nr_alloca(payload_arg_length + 1);
    nr_strxcpy(payload_string, payload_arg, payload_arg_length);

    if (type_arg) {
      type_string = (char*)nr_alloca(type_arg_length + 1);
      nr_strxcpy(type_string, type_arg, type_arg_length);
    }
  }

  if (!nr_php_recording(TSRMLS_C)) {
    RETURN_FALSE;
  }

  // args parsed, now do the work
  if (nr_php_api_accept_distributed_trace_payload(NRPRG(txn), payload_string,
                                                  type_string)) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}
