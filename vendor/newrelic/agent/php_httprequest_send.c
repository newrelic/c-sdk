#include "php_agent.h"
#include "php_call.h"
#include "php_hash.h"
#include "php_httprequest_send.h"
#include "nr_header.h"
#include "util_logging.h"
#include "util_memory.h"

/*
 * This pecl_http 1 instrumentation is currently not supported for Distributed
 * Tracing.
 */
void nr_php_httprequest_send_request_headers(zval* this_var TSRMLS_DC) {
  char* x_newrelic_id = 0;
  char* x_newrelic_transaction = 0;
  char* x_newrelic_synthetics = 0;
  char* newrelic = 0;

  if ((0 == this_var) || (0 == NRPRG(txn)->options.cross_process_enabled)) {
    return;
  }

  nr_header_outbound_request(NRPRG(txn), &x_newrelic_id,
                             &x_newrelic_transaction, &x_newrelic_synthetics,
                             &newrelic);

  if (NRPRG(txn) && NRTXN(special_flags.debug_cat)) {
    nrl_verbosedebug(
        NRL_CAT,
        "CAT: outbound request: transport='pecl_http 1' %s=" NRP_FMT
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

    retval = nr_php_call(this_var, "addHeaders", arr);

    nr_php_zval_free(&arr);
    nr_php_zval_free(&retval);
  }

  nr_free(x_newrelic_id);
  nr_free(x_newrelic_transaction);
  nr_free(x_newrelic_synthetics);
}

char* nr_php_httprequest_send_response_header(zval* this_var TSRMLS_DC) {
  zval* retval = 0;
  zval* header_name = 0;
  char* x_newrelic_app_data = 0;

  if ((0 == this_var) || (0 == NRPRG(txn)->options.cross_process_enabled)) {
    return 0;
  }

  header_name = nr_php_zval_alloc();
  /*
   * Though we use the lower case name here, it doesn't matter since
   * getResponseHeader will transform the string into the proper format.
   */
  nr_php_zval_str(header_name, X_NEWRELIC_APP_DATA_LOWERCASE);

  retval = nr_php_call(this_var, "getResponseHeader", header_name);
  if (nr_php_is_zval_non_empty_string(retval)) {
    x_newrelic_app_data = nr_strndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
  }

  nr_php_zval_free(&header_name);
  nr_php_zval_free(&retval);

  return x_newrelic_app_data;
}

char* nr_php_httprequest_send_get_url(zval* this_var TSRMLS_DC) {
  char* url = 0;
  zval* urlz = 0;

  if (0 == this_var) {
    return 0;
  }

  urlz = nr_php_call(this_var, "getUrl");
  if (nr_php_is_zval_non_empty_string(urlz)) {
    url = nr_strndup(Z_STRVAL_P(urlz), Z_STRLEN_P(urlz));
  }

  nr_php_zval_free(&urlz);

  return url;
}
