#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libnewrelic.h"

extern void run(newrelic_app_t* app);

#define RUN_APP() void run(newrelic_app_t* app)
#define RUN_NONWEB_TXN(M_txnname)                                              \
  void run_nonweb_transaction(newrelic_app_t* app __attribute__((__unused__)), \
                              newrelic_txn_t* txn);                            \
  void run(newrelic_app_t* app) {                                              \
    newrelic_txn_t* txn;                                                       \
    txn = newrelic_start_non_web_transaction(app, M_txnname);                  \
    assert(txn);                                                               \
    run_nonweb_transaction(app, txn);                                          \
    newrelic_end_transaction(&txn);                                            \
  }                                                                            \
  void run_nonweb_transaction(newrelic_app_t* app __attribute__((__unused__)), \
                              newrelic_txn_t* txn)

#define RUN_WEB_TXN(M_txnname)                                                \
  void run_web_transaction(newrelic_app_t* app __attribute__((__unused__),  \
                           newrelic_txn_t* txn);                            \
  void run(newrelic_app_t* app) {                                           \
    newrelic_txn_t* txn;                                                    \
    txn = newrelic_start_web_transaction(app, M_txnname);                   \
    assert(txn);                                                            \
    run_web_transaction(app, txn);                                          \
    newrelic_end_transaction(&txn);                                         \
  }                                                                         \
  void run_web_transaction(newrelic_app_t* app __attribute__((__unused__)), \
                           newrelic_txn_t* txn)

#define SAFE_GETENV(M_varname, M_default) \
  (getenv(M_varname) ? getenv(M_varname) : (M_default))

#define TAP_ASSERT(M_exp)            \
  if (M_exp) {                       \
    printf("ok - %s\n", #M_exp);     \
  } else {                           \
    printf("not ok - %s\n", #M_exp); \
  }

int main(int argc __attribute__((__unused__)),
         char* argv[] __attribute__((__unused__))) {
  newrelic_app_t* app;
  newrelic_app_config_t* cfg;

  newrelic_configure_log(SAFE_GETENV("NEW_RELIC_LOG_FILE", "./c_sdk.log"),
                         NEWRELIC_LOG_DEBUG);
  newrelic_init(SAFE_GETENV("NEW_RELIC_DAEMON_SOCKET", "/tmp/.newrelic.sock"),
                0);

  cfg = newrelic_create_app_config(NEW_RELIC_DAEMON_TESTNAME,
                                   SAFE_GETENV("NEW_RELIC_LICENSE_KEY", ""));
  assert(cfg);

  NEW_RELIC_CONFIG

  app = newrelic_create_app(cfg, 10000);
  assert(app);

  run(app);
  newrelic_destroy_app(&app);
  free(cfg);

  return 0;
}
