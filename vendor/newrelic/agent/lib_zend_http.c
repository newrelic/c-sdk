#include "php_agent.h"
#include "php_call.h"
#include "php_execute.h"
#include "php_hash.h"
#include "php_wrapper.h"
#include "fw_hooks.h"
#include "lib_zend_http.h"
#include "nr_header.h"
#include "nr_segment_external.h"
#include "util_logging.h"

typedef enum _nr_zend_http_adapter {
  NR_ZEND_ADAPTER_UNKNOWN = -1,
  NR_ZEND_ADAPTER_CURL = 0,
  NR_ZEND_ADAPTER_OTHER = 1,
} nr_zend_http_adapter;

/*
 * Purpose : Determine which HTTP client adapter is being used by a Zend
 *           external call.
 *
 * Params  : 1. A zval representing an instance of Zend_Http_Client.
 *
 * Returns : One of the nr_zend_http_adapter enum values.
 */
static nr_zend_http_adapter nr_zend_check_adapter(zval* this_var TSRMLS_DC) {
  zval* config = 0;
  zval* adapter_ivar = 0;
  zval* adapter_val = 0;
  const char* curl_adapter_typename = "Zend_Http_Client_Adapter_Curl";

  if (0 == this_var) {
    return NR_ZEND_ADAPTER_UNKNOWN;
  }

  /*
   * How we determine which adapter is being used.
   *   1) check if $adapter has been initialized
   *   2) if yes, check if it is an instance of the cURL adapter
   *   3) otherwise, check if the config hash contains an 'adapter' key
   *   4) if present, check whether its value is an instance of the cURL
   *      adapter or a string representing the cURL adapter's typename.
   */

  adapter_ivar = nr_php_get_zval_object_property(this_var, "adapter" TSRMLS_CC);
  if (nr_php_is_zval_valid_object(adapter_ivar)) {
    if (nr_php_object_instanceof_class(adapter_ivar,
                                       curl_adapter_typename TSRMLS_CC)) {
      nrl_verbosedebug(NRL_FRAMEWORK, "Zend: adapter is Curl");
      return NR_ZEND_ADAPTER_CURL;
    }
    return NR_ZEND_ADAPTER_OTHER;
  }

  config = nr_php_get_zval_object_property(this_var, "config" TSRMLS_CC);
  if ((0 == config) || (IS_ARRAY != Z_TYPE_P(config))) {
    nrl_verbosedebug(NRL_FRAMEWORK, "Zend: this->config is not array");
    return NR_ZEND_ADAPTER_UNKNOWN;
  }

  adapter_val = nr_php_zend_hash_find(Z_ARRVAL_P(config), "adapter");
  if (NULL == adapter_val) {
    nrl_verbosedebug(NRL_FRAMEWORK,
                     "Zend: unable to find adapter in this->config");
    return NR_ZEND_ADAPTER_UNKNOWN;
  }

  if (nr_php_is_zval_valid_string(adapter_val)) {
    if (0
        == nr_strncaseidx(Z_STRVAL_P(adapter_val), curl_adapter_typename,
                          Z_STRLEN_P(adapter_val))) {
      nrl_verbosedebug(NRL_FRAMEWORK, "Zend: adapter is Curl");
      return NR_ZEND_ADAPTER_CURL;
    }
    return NR_ZEND_ADAPTER_OTHER;
  }

  if (nr_php_is_zval_valid_object(adapter_val)) {
    if (nr_php_object_instanceof_class(adapter_val,
                                       curl_adapter_typename TSRMLS_CC)) {
      nrl_verbosedebug(NRL_FRAMEWORK, "Zend: adapter is Curl");
      return NR_ZEND_ADAPTER_CURL;
    }
    return NR_ZEND_ADAPTER_OTHER;
  }

  nrl_verbosedebug(NRL_FRAMEWORK,
                   "Zend: this->config['adapter'] is not string or object");
  return NR_ZEND_ADAPTER_UNKNOWN;
}

/*
 * Purpose : Get the url of a Zend_Http_Client instance before a
 *           Zend_Http_Client::request call.
 *
 * Params  : 1. The instance receiver of the Zend_Http_Client::request call.
 *           2. A pointer to a string that will receive the URL. It is the
 *              responsibility of the caller to free this URL.
 *
 * Returns : NR_SUCCESS or NR_FAILURE. If NR_FAILURE is returned, the url and
 *           url_len pointers will be unchanged.
 */
static nr_status_t nr_zend_http_client_request_get_url(zval* this_var,
                                                       char** url_ptr
                                                           TSRMLS_DC) {
  nr_status_t retval = NR_FAILURE;
  zval* uri = 0;
  zval* rval = 0;

  if ((0 == this_var) || (0 == url_ptr)) {
    goto end;
  }

  if (!nr_php_is_zval_valid_object(this_var)) {
    nrl_verbosedebug(NRL_FRAMEWORK, "Zend: this not an object: %d",
                     Z_TYPE_P(this_var));
    goto end;
  }

  uri = nr_php_get_zval_object_property(this_var, "uri" TSRMLS_CC);
  if (0 == uri) {
    nrl_verbosedebug(NRL_FRAMEWORK, "Zend: no URI");
    goto end;
  }

  if (0 == nr_php_object_instanceof_class(uri, "Zend_Uri_Http" TSRMLS_CC)) {
    if (nr_php_is_zval_valid_object(uri)) {
      nrl_verbosedebug(
          NRL_FRAMEWORK, "Zend: URI is wrong class: %*s.",
          NRSAFELEN(nr_php_class_entry_name_length(Z_OBJCE_P(uri))),
          nr_php_class_entry_name(Z_OBJCE_P(uri)));
    } else {
      nrl_verbosedebug(NRL_FRAMEWORK, "Zend: URI is not an object: %d",
                       Z_TYPE_P(uri));
    }
    goto end;
  }

  /*
   * Ok, this_var->uri seems to exist and seems to be of the right type. Now we
   * need to call uri->getUri() to get the name of the url to use.
   */

  rval = nr_php_call(uri, "getUri");
  if (nr_php_is_zval_non_empty_string(rval)) {
    *url_ptr = nr_strndup(Z_STRVAL_P(rval), Z_STRLEN_P(rval));
    retval = NR_SUCCESS;
  } else {
    nrl_verbosedebug(NRL_FRAMEWORK, "Zend: uri->getUri() failed");
  }

end:
  nr_php_zval_free(&rval);
  return retval;
}

/*
 * Purpose : Add the cross process request headers to a
 *           Zend_Http_Client::request call by using the
 *           Zend_Http_Client::setHeaders method.
 */
static void nr_zend_http_client_request_add_request_headers(
    zval* this_var TSRMLS_DC) {
  char* x_newrelic_id = 0;
  char* x_newrelic_transaction = 0;
  char* x_newrelic_synthetics = 0;
  char* newrelic = 0;

  if (0 == this_var) {
    return;
  }
  if (!nr_php_is_zval_valid_object(this_var)) {
    return;
  }

  nr_header_outbound_request(NRPRG(txn), &x_newrelic_id,
                             &x_newrelic_transaction, &x_newrelic_synthetics,
                             &newrelic);

  if (NRPRG(txn) && NRTXN(special_flags.debug_cat)) {
    nrl_verbosedebug(
        NRL_CAT,
        "CAT: outbound request: transport='Zend_Http_Client' %s=" NRP_FMT
        " %s=" NRP_FMT,
        X_NEWRELIC_ID, NRP_CAT(x_newrelic_id), X_NEWRELIC_TRANSACTION,
        NRP_CAT(x_newrelic_transaction));
  }

  if (x_newrelic_id && x_newrelic_transaction) {
    zval* arr = nr_php_zval_alloc();
    zval* retval = 0;

    array_init(arr);
    nr_php_add_assoc_string(arr, X_NEWRELIC_ID, x_newrelic_id);
    nr_php_add_assoc_string(arr, X_NEWRELIC_TRANSACTION,
                            x_newrelic_transaction);

    if (x_newrelic_synthetics) {
      nr_php_add_assoc_string(arr, X_NEWRELIC_SYNTHETICS,
                              x_newrelic_synthetics);
    }

    retval = nr_php_call(this_var, "setHeaders", arr);

    nr_php_zval_free(&arr);
    nr_php_zval_free(&retval);
  }

  nr_free(x_newrelic_id);
  nr_free(x_newrelic_transaction);
  nr_free(x_newrelic_synthetics);
}

/*
 * Purpose : Get the cross process response header after a
 *           Zend_Http_Client::request call by using the
 *           Zend_Http_Response::getHeader method.
 */
static char* nr_zend_http_client_request_get_response_header(
    zval* response TSRMLS_DC) {
  zval* header_name = 0;
  zval* retval = 0;
  char* response_header = 0;

  if (0 == NRPRG(txn)) {
    return 0;
  }
  if (0 == NRPRG(txn)->options.cross_process_enabled) {
    return 0;
  }

  if (0 == nr_php_is_zval_valid_object(response)) {
    return 0;
  }

  header_name = nr_php_zval_alloc();
  nr_php_zval_str(header_name, X_NEWRELIC_APP_DATA);

  retval = nr_php_call(response, "getHeader", header_name);

  nr_php_zval_free(&header_name);
  if (1 == nr_php_is_zval_non_empty_string(retval)) {
    response_header = nr_strndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
  }

  nr_php_zval_free(&retval);

  return response_header;
}

/*
 * Wrap and record external metrics for Zend_Http_Client::request.
 *
 * http://framework.zend.com/manual/1.12/en/zend.http.client.advanced.html
 */
NR_PHP_WRAPPER_START(nr_zend_http_client_request) {
  zval* this_var = 0;
  zval** retval_ptr;
  nr_segment_t* segment;
  nr_segment_external_params_t external_params
      = {.library = "Zend_Http_Client"};
  nr_zend_http_adapter adapter;

  (void)wraprec;

  this_var = nr_php_scope_get(NR_EXECUTE_ORIG_ARGS TSRMLS_CC);
  retval_ptr = nr_php_get_return_value_ptr(TSRMLS_C);

  /* Avoid double counting if CURL is used. */
  adapter = nr_zend_check_adapter(this_var TSRMLS_CC);
  if ((NR_ZEND_ADAPTER_CURL == adapter)
      || (NR_ZEND_ADAPTER_UNKNOWN == adapter)) {
    NR_PHP_WRAPPER_CALL;
    goto leave;
  }

  if (NR_FAILURE
      == nr_zend_http_client_request_get_url(this_var,
                                             &external_params.uri TSRMLS_CC)) {
    NR_PHP_WRAPPER_CALL;
    goto leave;
  }

  nr_zend_http_client_request_add_request_headers(this_var TSRMLS_CC);

  segment = nr_segment_start(NRPRG(txn), NULL, NULL);
  NR_PHP_WRAPPER_CALL;

  if (retval_ptr) {
    external_params.encoded_response_header
        = nr_zend_http_client_request_get_response_header(
            *retval_ptr TSRMLS_CC);
  } else {
    nrl_verbosedebug(NRL_FRAMEWORK,
                     "Zend: unable to obtain return value from request");
  }

  if (NRPRG(txn) && NRTXN(special_flags.debug_cat)) {
    nrl_verbosedebug(
        NRL_CAT,
        "CAT: outbound response: transport='Zend_Http_Client' %s=" NRP_FMT,
        X_NEWRELIC_APP_DATA, NRP_CAT(external_params.encoded_response_header));
  }

  nr_segment_external_end(segment, &external_params);

leave:
  nr_free(external_params.encoded_response_header);
  nr_free(external_params.uri);
  nr_php_scope_release(&this_var);
}
NR_PHP_WRAPPER_END

void nr_zend_http_enable(TSRMLS_D) {
  if (NR_FW_ZEND != NRPRG(current_framework)) {
    nr_php_wrap_user_function(NR_PSTR("Zend_Http_Client::request"),
                              nr_zend_http_client_request TSRMLS_CC);
  }
}
