#include "php_agent.h"
#include "php_internal_instrument.h"
#include "php_user_instrument.h"
#include "php_execute.h"
#include "fw_support.h"
#include "fw_hooks.h"
#include "util_memory.h"
#include "util_strings.h"

/*
 * Determine the WT name from the CodeIgniter dispatcher.
 * Usage: called from a specific internal function wrapper
 */
static void nr_codeigniter_name_the_wt(zend_function* func,
                                       const zend_function* caller NRUNUSED
                                           TSRMLS_DC) {
  zend_op_array* op_array = nr_php_current_op_array(TSRMLS_C);

  if ((NR_FW_CODEIGNITER != NRPRG(current_framework) || (NULL == func)
       || (NULL == func->common.scope))) {
    return;
  }

  /*
   * We're looking for a particular active call stack:
   *   1. (php file) CodeIgniter.php
   *   ..calls..
   *   2. (internal function) call_user_func_array( <action>, ... )
   */
  if (OP_ARRAY_IS_FILE(op_array, "CodeIgniter.php")) {
    char* action = NULL;
    zend_class_entry* ce = func->common.scope;

    /*
     * the codeigniter name is the class and method being passed as an
     * array as first parameter to call_user_func_array
     */

    action = nr_formatf("%.*s/%.*s", (int)nr_php_class_entry_name_length(ce),
                        nr_php_class_entry_name(ce),
                        (int)nr_php_function_name_length(func),
                        nr_php_function_name(func));

    nr_txn_set_path("CodeIgniter", NRPRG(txn), action, NR_PATH_TYPE_ACTION,
                    NR_NOT_OK_TO_OVERWRITE);

    nr_free(action);
  }
}

/*
 * Enable CodeIgniter instrumentation
 */
void nr_codeigniter_enable(TSRMLS_D) {
  nr_php_add_call_user_func_array_pre_callback(
      nr_codeigniter_name_the_wt TSRMLS_CC);
}
