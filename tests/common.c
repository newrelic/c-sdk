#include "test.h"
#include "libnewrelic.h"
#include "config.h"
#include "transaction.h"

#include "app.h"
#include "nr_txn.h"
#include "nr_segment_private.h"
#include "util_memory.h"

/*! @brief Provides a fake app ready to be used in a transaction. */
int app_group_setup(void** state) {
  newrelic_app_t* app;
  nrapp_t* nrapp;
  nr_app_info_t* app_info;

  nrapp = (nrapp_t*)nr_zalloc(sizeof(nrapp_t));
  nrapp->state = NR_APP_OK;

  app_info = (nr_app_info_t*)nr_zalloc(sizeof(nr_app_info_t));

  app = (newrelic_app_t*)nr_zalloc(sizeof(newrelic_app_t));
  app->app_info = app_info;
  app->app = nrapp;

  *state = app;
  return 0;  // tells cmocka setup completed, 0==OK
}

/*! @brief Provides a fake transaction as the group state. */
int txn_group_setup(void** state) {
  newrelic_txn_t* txn = 0;
  nrtxnopt_t* opts = 0;
  txn = (newrelic_txn_t*)nr_zalloc(sizeof(newrelic_txn_t));
  nrt_mutex_init(&txn->lock, 0);
  txn->txn = (nrtxn_t*)nr_zalloc(sizeof(nrtxn_t));
  opts = newrelic_get_default_options();
  txn->txn->options = *opts;
  nr_free(opts);
  txn->txn->status.recording = 1;

  txn->txn->scoped_metrics = nrm_table_create(5);
  txn->txn->unscoped_metrics = nrm_table_create(5);

  txn->txn->trace_strings = nr_string_pool_create();
  nr_stack_init(&txn->txn->parent_stack, NR_STACK_DEFAULT_CAPACITY);

  txn->txn->segment_root = nr_zalloc(sizeof(nr_segment_t));
  txn->txn->segment_root->txn = txn->txn;
  nr_segment_children_init(&txn->txn->segment_root->children);
  txn->txn->segment_root->start_time = 0;
  txn->txn->segment_root->stop_time = 0;
  nr_txn_set_current_segment(txn->txn, txn->txn->segment_root);
  txn->txn->segment_count = 1;

  *state = txn;
  return 0;  // tells cmocka setup completed, 0==OK
}

/*! @brief Cleans up the fake app provided in thhe group state. */
int app_group_teardown(void** state) {
  newrelic_app_t* appWithInfo;
  appWithInfo = (newrelic_app_t*)*state;
  nr_free(appWithInfo->app);
  newrelic_destroy_app(&appWithInfo);
  return 0;  // tells cmocka teardown completed, 0==OK
}

/*! @brief Cleans up the fake transaction provided in the group state. */
int txn_group_teardown(void** state) {
  newrelic_txn_t* txn = 0;
  txn = (newrelic_txn_t*)*state;

  nrt_mutex_destroy(&txn->lock);

  nr_txn_destroy(&txn->txn);
  nr_free(txn);

  return 0;  // tells cmocka teardown completed, 0==OK
}
