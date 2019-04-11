#include "libnewrelic.h"
#include "stack.h"
#include "transaction.h"

#include "nr_txn.h"
#include "util_logging.h"
#include "util_memory.h"

void newrelic_notice_error(newrelic_txn_t* transaction,
                           int priority,
                           const char* errmsg,
                           const char* errclass) {
  if (NULL == transaction) {
    nrl_error(NRL_INSTRUMENT, "unable to add error to NULL transaction");
    return;
  }

  if (NULL == errmsg || 0 == errmsg[0]) {
    nrl_error(NRL_INSTRUMENT,
              "unable to add NULL/empty error message to transaction");
    return;
  }

  if (NULL == errclass || 0 == errclass[0]) {
    nrl_error(NRL_INSTRUMENT,
              "unable to add NULL/empty error class to transaction");
    return;
  }

  nrt_mutex_lock(&transaction->lock);
  {
    char* stacktrace_json;

    if (0 == transaction->txn->options.err_enabled) {
      nrl_error(NRL_INSTRUMENT,
                "unable to add error to transaction when errors are disabled");
      goto end;
    }

    if (0 == transaction->txn->status.recording) {
      nrl_error(NRL_INSTRUMENT,
                "unable to add error to transaction that is not recording");
      goto end;
    }

    if (0 != transaction->txn->error
        && priority < nr_error_priority(transaction->txn->error)) {
      nrl_error(
          NRL_INSTRUMENT,
          "an error with a higher priority already exists on transaction");
      goto end;
    }

    if (NR_FAILURE == nr_txn_record_error_worthy(transaction->txn, priority)) {
      nrl_error(NRL_INSTRUMENT, "unable to add error to transaction");
      goto end;
    }

    stacktrace_json = newrelic_get_stack_trace_as_json();
    nr_txn_record_error(transaction->txn, priority, errmsg, errclass,
                        stacktrace_json);
    nr_free(stacktrace_json);
  }
end:
  nrt_mutex_unlock(&transaction->lock);
}
