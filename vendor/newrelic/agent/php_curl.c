#include "php_agent.h"
#include "php_call.h"
#include "php_curl.h"
#include "php_hash.h"
#include "php_user_instrument.h"
#include "php_wrapper.h"
#include "nr_header.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_metrics.h"
#include "util_strings.h"

static int nr_php_curl_do_cross_process(TSRMLS_D) {
  if (0 == nr_php_recording(TSRMLS_C)) {
    return 0;
  }
  return (NRPRG(txn)->options.cross_process_enabled
          || NRPRG(txn)->options.distributed_tracing_enabled);
}

static void nr_php_curl_save_response_header_from_zval(
    const zval* zstr TSRMLS_DC) {
  char* str;
  char* hdr;

  if ((0 == zstr) || (0 == nr_php_is_zval_non_empty_string(zstr))) {
    return;
  }

  if (0 == nr_php_curl_do_cross_process(TSRMLS_C)) {
    return;
  }

  str = nr_strndup(Z_STRVAL_P(zstr), Z_STRLEN_P(zstr)); /* NUL-terminate it */
  hdr = nr_header_extract_encoded_value(X_NEWRELIC_APP_DATA, str);
  nr_free(str);

  if (0 == hdr) {
    return;
  }

  nr_free(NRPRG(curl_exec_x_newrelic_app_data));
  NRPRG(curl_exec_x_newrelic_app_data) = hdr;
}

static void nr_php_curl_header_destroy(zval* header) {
  nr_php_zval_free(&header);
}

static inline nr_hashmap_t* nr_php_curl_get_headers(TSRMLS_D) {
  if (NULL == NRPRG(curl_headers)) {
    NRPRG(curl_headers)
        = nr_hashmap_create((nr_hashmap_dtor_func_t)nr_php_curl_header_destroy);
  }
  return NRPRG(curl_headers);
}

static void nr_php_curl_method_destroy(char* method) {
  nr_free(method);
}

static inline nr_hashmap_t* nr_php_curl_get_method_hash(TSRMLS_D) {
  if (NULL == NRPRG(curl_method)) {
    NRPRG(curl_method)
        = nr_hashmap_create((nr_hashmap_dtor_func_t)nr_php_curl_method_destroy);
  }
  return NRPRG(curl_method);
}

/*
 * This wrapper should be attached to any function which has been set as
 * callback to receive curl_exec headers (set using curl_setopt). The callback
 * is expected to have two parameters: The curl resource and a string containing
 * header data.
 */
NR_PHP_WRAPPER(nr_php_curl_user_header_callback) {
  zval* headers = nr_php_arg_get(2, NR_EXECUTE_ORIG_ARGS TSRMLS_CC);

  (void)wraprec;

  nr_php_curl_save_response_header_from_zval(headers TSRMLS_CC);

  NR_PHP_WRAPPER_CALL;

  nr_php_arg_release(&headers);
}
NR_PHP_WRAPPER_END

#define NR_CURL_RESPONSE_HEADER_CALLBACK_NAME "newrelic_curl_header_callback"
#ifdef TAGS
void zif_newrelic_curl_header_callback(void); /* ctags landing pad only */
void newrelic_curl_header_callback(void);     /* ctags landing pad only */
#endif
PHP_FUNCTION(newrelic_curl_header_callback) {
  zval* curl_resource = 0;
  zval* header_data = 0;
  int rv;

  NR_UNUSED_HT;
  NR_UNUSED_RETURN_VALUE_PTR;
  NR_UNUSED_RETURN_VALUE_USED;
  NR_UNUSED_THIS_PTR;

  rv = zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
                                ZEND_NUM_ARGS() TSRMLS_CC, "zz", &curl_resource,
                                &header_data);

  /*
   * This callback is expected to return the length of the header_data received.
   */
  if (nr_php_is_zval_non_empty_string(header_data)) {
    RETVAL_LONG(Z_STRLEN_P(header_data));
  } else {
    RETVAL_LONG(0);
  }

  if (SUCCESS != rv) {
    return;
  }

  nr_php_curl_save_response_header_from_zval(header_data TSRMLS_CC);
}

static void nr_php_curl_set_default_response_header_callback(
    zval* curlres TSRMLS_DC) {
  zval* callback_name;
  zval* retval = 0;
  zval* curlopt;

  if ((0 == curlres) || (IS_RESOURCE != Z_TYPE_P(curlres))) {
    return;
  }

  curlopt = nr_php_get_constant("CURLOPT_HEADERFUNCTION" TSRMLS_CC);
  if (0 == curlopt) {
    return;
  }

  callback_name = nr_php_zval_alloc();
  nr_php_zval_str(callback_name, NR_CURL_RESPONSE_HEADER_CALLBACK_NAME);

  retval = nr_php_call(NULL, "curl_setopt", curlres, curlopt, callback_name);

  nr_php_zval_free(&retval);
  nr_php_zval_free(&callback_name);
  nr_php_zval_free(&curlopt);
}

static void nr_php_curl_set_default_request_headers(zval* curlres TSRMLS_DC) {
  zval* arr;
  zval* retval = 0;
  zval* curlopt;

  if ((0 == curlres) || (IS_RESOURCE != Z_TYPE_P(curlres))) {
    return;
  }

  curlopt = nr_php_get_constant("CURLOPT_HTTPHEADER" TSRMLS_CC);
  if (0 == curlopt) {
    return;
  }

  arr = nr_php_zval_alloc();
  array_init(arr);
  /*
   * Note that we do not need to populate the 'arr' parameter with the
   * New Relic headers as those are added by the curl_setopt instrumentation.
   */

  retval = nr_php_call(NULL, "curl_setopt", curlres, curlopt, arr);

  nr_php_zval_free(&retval);
  nr_php_zval_free(&arr);
  nr_php_zval_free(&curlopt);
}

void nr_php_curl_init(zval* curlres TSRMLS_DC) {
  if (0 == nr_php_curl_do_cross_process(TSRMLS_C)) {
    return;
  }

  nr_php_curl_set_default_response_header_callback(curlres TSRMLS_CC);
  nr_php_curl_set_default_request_headers(curlres TSRMLS_CC);
}

static int nr_php_curl_copy_outbound_headers_iterator(zval* element,
                                                      zval* dest,
                                                      zend_hash_key* key
                                                          NRUNUSED TSRMLS_DC) {
  NR_UNUSED_TSRMLS;

  /*
   * Do nothing for New Relic headers, which are always strings.
   */
  if (nr_php_is_zval_valid_string(element)
      && ((nr_strncaseidx(Z_STRVAL_P(element), X_NEWRELIC_ID,
                          Z_STRLEN_P(element))
           >= 0)
          || (nr_strncaseidx(Z_STRVAL_P(element), X_NEWRELIC_TRANSACTION,
                             Z_STRLEN_P(element))
              >= 0)
          || (nr_strncaseidx(Z_STRVAL_P(element), X_NEWRELIC_SYNTHETICS,
                             Z_STRLEN_P(element))
              >= 0)
          || (nr_strncaseidx(Z_STRVAL_P(element), NEWRELIC, Z_STRLEN_P(element))
              >= 0))) {
    return ZEND_HASH_APPLY_KEEP;
  }

  /*
   * Otherwise, we copy the header into the destination array, being careful
   * to increment the refcount on the element to avoid double frees.
   */
#ifdef PHP7
  if (Z_REFCOUNTED_P(element)) {
    Z_ADDREF_P(element);
  }
#else
  Z_ADDREF_P(element);
#endif
  add_next_index_zval(dest, element);

  return ZEND_HASH_APPLY_KEEP;
}

char* nr_php_curl_exec_get_method(zval* curlres TSRMLS_DC) {
  char* method = nr_hashmap_index_get(nr_php_curl_get_method_hash(TSRMLS_C),
                                      nr_php_zval_resource_id(curlres));
  return method;
}

void nr_php_curl_exec_set_httpheaders(zval* curlres TSRMLS_DC) {
  zval* headers = NULL;
  int old_curl_ignore_setopt = NRPRG(curl_ignore_setopt);
  zval* retval = NULL;
  char* x_newrelic_id = 0;
  char* x_newrelic_transaction = 0;
  char* x_newrelic_synthetics = 0;
  char* newrelic = 0;
  zval* curlopt;
  zval* curlval = nr_hashmap_index_get(nr_php_curl_get_headers(TSRMLS_C),
                                       nr_php_zval_resource_id(curlres));

  if ((0 == curlval) || (IS_ARRAY != Z_TYPE_P(curlval))) {
    nrl_warning(NRL_CAT,
                "Could not instrument curl handle, it may have been "
                "initialized in a different transaction.");
    return;
  }

  curlopt = nr_php_get_constant("CURLOPT_HTTPHEADER" TSRMLS_CC);
  if (NULL == curlopt) {
    return;
  }

  /*
   * Copy the parameter headers into a new array, removing any existing New
   * Relic headers along the way to prevent header accumulation.
   */
  headers = nr_php_zval_alloc();
  array_init(headers);

  nr_php_zend_hash_zval_apply(
      Z_ARRVAL_P(curlval),
      (nr_php_zval_apply_t)nr_php_curl_copy_outbound_headers_iterator,
      headers TSRMLS_CC);

  nr_header_outbound_request(NRPRG(txn), &x_newrelic_id,
                             &x_newrelic_transaction, &x_newrelic_synthetics,
                             &newrelic);

  if (NRPRG(txn) && NRTXN(special_flags.debug_cat)) {
    nrl_verbosedebug(NRL_CAT,
                     "CAT: outbound request: transport='curl' %s=" NRP_FMT
                     " %s=" NRP_FMT,
                     X_NEWRELIC_ID, NRP_CAT(x_newrelic_id),
                     X_NEWRELIC_TRANSACTION, NRP_CAT(x_newrelic_transaction));
  }

  if (x_newrelic_id && x_newrelic_transaction) {
    char* h1 = nr_header_format_name_value(X_NEWRELIC_ID, x_newrelic_id, 0);
    char* h2 = nr_header_format_name_value(X_NEWRELIC_TRANSACTION,
                                           x_newrelic_transaction, 0);

    nr_php_add_next_index_string(headers, h1);
    nr_php_add_next_index_string(headers, h2);

    nr_free(h1);
    nr_free(h2);
  }

  if (x_newrelic_synthetics) {
    char* h3 = nr_header_format_name_value(X_NEWRELIC_SYNTHETICS,
                                           x_newrelic_synthetics, 0);

    nr_php_add_next_index_string(headers, h3);

    nr_free(h3);
  }

  if (newrelic) {
    char* h4 = nr_header_format_name_value(NEWRELIC, newrelic, 0);

    nr_php_add_next_index_string(headers, h4);

    nr_free(h4);
  }

  /*
   * Call curl_setopt() with the modified headers, taking care to set the
   * curl_ignore_setopt flag to avoid infinite recursion.
   */
  NRPRG(curl_ignore_setopt) = 1;
  retval = nr_php_call(NULL, "curl_setopt", curlres, curlopt, headers);
  if (!nr_php_is_zval_true(retval)) {
    nrl_verbosedebug(NRL_INSTRUMENT, "%s: error calling curl_setopt", __func__);
  }

  /*
   * We're done. Whether we were successful or not, let's clean up and return.
   */
  NRPRG(curl_ignore_setopt) = old_curl_ignore_setopt;
  nr_php_zval_free(&headers);
  nr_php_zval_free(&retval);
  nr_php_zval_free(&curlopt);
  nr_free(newrelic);
  nr_free(x_newrelic_id);
  nr_free(x_newrelic_transaction);
  nr_free(x_newrelic_synthetics);
}

static void nr_php_curl_setopt_curlopt_writeheader(zval* curlval TSRMLS_DC) {
  if ((0 == curlval) || (IS_RESOURCE != Z_TYPE_P(curlval))) {
    return;
  }

  /*
   * The user is setting a file to get the response headers.  This use case is
   * not currently supported.  Unfortunately, there does not seem to be any
   * simple solution: Adding a filter to the file stream may not work because
   * the filter could be applied long after the curl_exec call has finished.
   * TODO(willhf)
   */
  nrm_force_add(NRPRG(txn) ? NRPRG(txn)->unscoped_metrics : 0,
                "Supportability/Unsupported/curl_setopt/CURLOPT_WRITEHEADER",
                0);
}

static void nr_php_curl_setopt_curlopt_headerfunction(zval* curlval TSRMLS_DC) {
  const char* our_callback = NR_CURL_RESPONSE_HEADER_CALLBACK_NAME;
  nr_string_len_t our_callback_len
      = sizeof(NR_CURL_RESPONSE_HEADER_CALLBACK_NAME) - 1;

  if (0 == curlval) {
    return;
  }

  if (nr_php_is_zval_valid_object(curlval)) {
    /*
     * Here the user could be setting the callback function to an anonymous
     * closure.  This case is not yet supported.
     * TODO(willhf)
     */
    nrm_force_add(
        NRPRG(txn) ? NRPRG(txn)->unscoped_metrics : 0,
        "Supportability/Unsupported/curl_setopt/CURLOPT_HEADERFUNCTION/closure",
        0);
    return;
  }

  if (!nr_php_is_zval_valid_string(curlval)) {
    return;
  }

  if ((our_callback_len == Z_STRLEN_P(curlval))
      && (0
          == nr_strncmp(our_callback, Z_STRVAL_P(curlval), our_callback_len))) {
    /*
     * Abort if curl_setopt is being used to set our callback function as the
     * function to receive the response headers.  Note that we cannot put a
     * wraprec around our callback to gather the response header because our
     * callback is an internal function.
     */
    return;
  }

  nr_php_wrap_user_function(Z_STRVAL_P(curlval), (size_t)Z_STRLEN_P(curlval),
                            nr_php_curl_user_header_callback TSRMLS_CC);
}

void nr_php_curl_setopt_pre(zval* curlres,
                            zval* curlopt,
                            zval* curlval TSRMLS_DC) {
  if (0 == nr_php_curl_do_cross_process(TSRMLS_C)) {
    return;
  }

  if ((0 == curlres) || (0 == curlopt) || (0 == curlval)
      || (IS_RESOURCE != Z_TYPE_P(curlres)) || (IS_LONG != Z_TYPE_P(curlopt))) {
    return;
  }

  if (nr_php_is_zval_named_constant(curlopt, "CURLOPT_WRITEHEADER" TSRMLS_CC)) {
    nr_php_curl_setopt_curlopt_writeheader(curlval TSRMLS_CC);
    return;
  }

  if (nr_php_is_zval_named_constant(curlopt,
                                    "CURLOPT_HEADERFUNCTION" TSRMLS_CC)) {
    nr_php_curl_setopt_curlopt_headerfunction(curlval TSRMLS_CC);
    return;
  }
}

void nr_php_curl_setopt_post(zval* curlres,
                             zval* curlopt,
                             zval* curlval TSRMLS_DC) {
  char* method = NULL;

  if (0 == nr_php_curl_do_cross_process(TSRMLS_C)) {
    return;
  }

  if ((0 == curlres) || (0 == curlopt) || (0 == curlval)
      || (IS_RESOURCE != Z_TYPE_P(curlres)) || (IS_LONG != Z_TYPE_P(curlopt))) {
    return;
  }

  if (nr_php_is_zval_named_constant(curlopt, "CURLOPT_HTTPHEADER" TSRMLS_CC)) {
    zval* headers = nr_php_zval_alloc();
    array_init(headers);
    HashTable* ht = NULL;

    ht = HASH_OF(curlval);
    if (NULL == ht) {
      return;
    }

    // Save the headers
    nr_php_zend_hash_zval_apply(
        ht, (nr_php_zval_apply_t)nr_php_curl_copy_outbound_headers_iterator,
        headers TSRMLS_CC);
    nr_hashmap_index_update(nr_php_curl_get_headers(TSRMLS_C),
                            nr_php_zval_resource_id(curlres), headers);

  } else if (nr_php_is_zval_named_constant(curlopt, "CURLOPT_POST" TSRMLS_CC)) {
    method = nr_strdup("POST");
  } else if (nr_php_is_zval_named_constant(curlopt, "CURLOPT_PUT" TSRMLS_CC)) {
    method = nr_strdup("PUT");
  } else if (nr_php_is_zval_named_constant(curlopt,
                                           "CURLOPT_CUSTOMREQUEST" TSRMLS_CC)) {
    if (nr_php_is_zval_valid_string(curlval)) {
      method = nr_strndup(Z_STRVAL_P(curlval), Z_STRLEN_P(curlval));
    }
  } else if (nr_php_is_zval_named_constant(curlopt,
                                           "CURLOPT_HTTPGET" TSRMLS_CC)) {
    method = nr_strdup("GET");
  }

  if (NULL != method) {
    nr_hashmap_index_update(nr_php_curl_get_method_hash(TSRMLS_C),
                            nr_php_zval_resource_id(curlres), method);
  }
}

typedef struct _nr_php_curl_setopt_array_apply_t {
  zval* curlres;
  nr_php_curl_setopt_func_t func;
} nr_php_curl_setopt_array_apply_t;

static int nr_php_curl_setopt_array_apply(zval* value,
                                          nr_php_curl_setopt_array_apply_t* app,
                                          zend_hash_key* hash_key TSRMLS_DC) {
  zval* key = nr_php_zval_alloc();

  if (nr_php_zend_hash_key_is_string(hash_key)) {
    nr_php_zval_str(key, nr_php_zend_hash_key_string_value(hash_key));
  } else if (nr_php_zend_hash_key_is_numeric(hash_key)) {
    ZVAL_LONG(key, (zend_long)nr_php_zend_hash_key_integer(hash_key));
  } else {
    /*
     * This is a warning because this really, really shouldn't ever happen.
     */
    nrl_warning(NRL_INSTRUMENT, "%s: unexpected key type", __func__);
    goto end;
  }

  /*
   * Actually invoke the pre/post function.
   */
  (app->func)(app->curlres, key, value TSRMLS_CC);

end:
  nr_php_zval_free(&key);
  return ZEND_HASH_APPLY_KEEP;
}

void nr_php_curl_setopt_array(zval* curlres,
                              zval* options,
                              nr_php_curl_setopt_func_t func TSRMLS_DC) {
  nr_php_curl_setopt_array_apply_t app = {
      .curlres = curlres,
      .func = func,
  };

  if (!nr_php_is_zval_valid_resource(curlres)
      || !nr_php_is_zval_valid_array(options)) {
    return;
  }

  nr_php_zend_hash_zval_apply(
      Z_ARRVAL_P(options), (nr_php_zval_apply_t)nr_php_curl_setopt_array_apply,
      (void*)&app TSRMLS_CC);
}

char* nr_php_curl_get_url(zval* curlres TSRMLS_DC) {
  zval* retval = 0;
  char* url = 0;
  zval* curlinfo_effective_url;

  /*
   * Note that we do not check cross process enabled here.  The url is used
   * for curl instrumentation regardless of whether or not cross process is
   * enabled.
   */

  curlinfo_effective_url
      = nr_php_get_constant("CURLINFO_EFFECTIVE_URL" TSRMLS_CC);
  if (0 == curlinfo_effective_url) {
    return 0;
  }

  retval = nr_php_call(NULL, "curl_getinfo", curlres, curlinfo_effective_url);
  if (nr_php_is_zval_non_empty_string(retval)) {
    url = nr_strndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
  }

  nr_php_zval_free(&retval);
  nr_php_zval_free(&curlinfo_effective_url);
  return url;
}

/*
 * This function effectively wraps a blacklist of protocols to ignore.
 *
 *   FILE - ignored because use of the FILE protocol does not involve
 *          any network activity, and because the url is a local
 *          filesystem path. The latter is dangerous because it can
 *          lead to an unbounded number of unique external metrics.
 */
int nr_php_curl_should_instrument_proto(const char* url) {
  return 0 != nr_strncmp(url, NR_PSTR("file://"));
}
