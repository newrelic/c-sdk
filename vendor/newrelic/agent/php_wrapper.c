#include "php_agent.h"
#include "php_user_instrument.h"
#include "php_wrapper.h"
#include "util_logging.h"

nruserfn_t* nr_php_wrap_user_function(const char* name,
                                      size_t namelen,
                                      nrspecialfn_t callback TSRMLS_DC) {
  nruserfn_t* wraprec = nr_php_add_custom_tracer_named(name, namelen TSRMLS_CC);

  if (wraprec && callback) {
    if ((NULL != wraprec->special_instrumentation)
        && (callback != wraprec->special_instrumentation)) {
      nrl_verbosedebug(
          NRL_INSTRUMENT,
          "%s: attempting to set special_instrumentation for %.*s, but "
          "it is already set",
          __func__, NRSAFELEN(namelen), NRBLANKSTR(name));
    } else {
      wraprec->special_instrumentation = callback;
    }
  }

  return wraprec;
}

nruserfn_t* nr_php_wrap_user_function_extra(const char* name,
                                            size_t namelen,
                                            nrspecialfn_t callback,
                                            const char* extra TSRMLS_DC) {
  nruserfn_t* wraprec
      = nr_php_wrap_user_function(name, namelen, callback TSRMLS_CC);
  wraprec->extra = extra;

  return wraprec;
}

nruserfn_t* nr_php_wrap_callable(zend_function* callable,
                                 nrspecialfn_t callback TSRMLS_DC) {
  nruserfn_t* wraprec = nr_php_add_custom_tracer_callable(callable TSRMLS_CC);

  if (wraprec && callback) {
    if ((NULL != wraprec->special_instrumentation)
        && (callback != wraprec->special_instrumentation)) {
      nrl_verbosedebug(NRL_INSTRUMENT,
                       "%s: attempting to set special_instrumentation, but "
                       "it is already set",
                       __func__);
    } else {
      wraprec->special_instrumentation = callback;
    }
  }

  return wraprec;
}

inline static void release_zval(zval** ppzv) {
#ifdef PHP7
  nr_php_zval_free(ppzv);
#else
  if (NULL == ppzv) {
    return;
  }
  if (NULL == *ppzv) {
    return;
  }

  zval_ptr_dtor(ppzv);
  *ppzv = NULL;
#endif /* PHP7 */
}

zval* nr_php_arg_get(ssize_t index, NR_EXECUTE_PROTO TSRMLS_DC) {
  zval* arg;

#ifdef PHP7
  {
    zval* orig;

    arg = NULL;
    orig = nr_php_get_user_func_arg((zend_uint)index, NR_EXECUTE_ORIG_ARGS);

    if (orig) {
      arg = nr_php_zval_alloc();
      ZVAL_DUP(arg, orig);
    }
  }
#else
  arg = nr_php_get_user_func_arg((zend_uint)index,
                                 NR_EXECUTE_ORIG_ARGS TSRMLS_CC);

  if (arg) {
    Z_ADDREF_P(arg);
  }
#endif /* PHP7 */

  return arg;
}

void nr_php_arg_release(zval** ppzv) {
  release_zval(ppzv);
}

zval* nr_php_scope_get(NR_EXECUTE_PROTO TSRMLS_DC) {
  zval* this_ptr;

#ifdef PHP7
  this_ptr = nr_php_zval_alloc();
  ZVAL_DUP(this_ptr, NR_PHP_USER_FN_THIS);
#else
  NR_UNUSED_SPECIALFN;

  this_ptr = NR_PHP_USER_FN_THIS;
  Z_ADDREF_P(this_ptr);
#endif

  return this_ptr;
}

void nr_php_scope_release(zval** ppzv) {
  release_zval(ppzv);
}

zval** nr_php_get_return_value_ptr(TSRMLS_D) {
#ifdef PHP7
  if (NULL == EG(current_execute_data)) {
    return NULL;
  }

  return &EG(current_execute_data)->return_value;
#else
  return EG(return_value_ptr_ptr);
#endif /* PHP7 */
}
