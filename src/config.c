#include "libnewrelic.h"
#include "config.h"

#include "util_logging.h"
#include "util_memory.h"
#include "util_strings.h"

nr_tt_recordsql_t newrelic_validate_recordsql(newrelic_tt_recordsql_t setting) {
  if (NEWRELIC_SQL_OFF == setting) {
    return NR_SQL_NONE;
  }
  if (NEWRELIC_SQL_RAW == setting) {
    return NR_SQL_RAW;
  }
  if (NEWRELIC_SQL_OBFUSCATED == setting) {
    return NR_SQL_OBFUSCATED;
  }
  return NR_SQL_OBFUSCATED;
}

newrelic_app_config_t* newrelic_create_app_config(const char* app_name,
                                                  const char* license_key) {
  newrelic_app_config_t* config;

  if (NULL == app_name) {
    nrl_error(NRL_INSTRUMENT, "app name is required");
    return NULL;
  }

  if (NR_LICENSE_SIZE != nr_strlen(license_key)) {
    nrl_error(NRL_INSTRUMENT, "invalid license key format");
    return NULL;
  }

  config = (newrelic_app_config_t*)nr_zalloc(sizeof(newrelic_app_config_t));
  nr_strxcpy(config->app_name, app_name, nr_strlen(app_name));
  nr_strxcpy(config->license_key, license_key, nr_strlen(license_key));

  /* Set up the default transaction tracer configuration. */
  config->transaction_tracer.enabled = true;
  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_APDEX_FAILING;
  config->transaction_tracer.duration_us = 0;
  config->transaction_tracer.stack_trace_threshold_us = 500000;

  /* Set up the default datastore reporting configuration */
  config->transaction_tracer.datastore_reporting.enabled = true;
  config->transaction_tracer.datastore_reporting.threshold_us = 500000;
  config->transaction_tracer.datastore_reporting.record_sql
      = NEWRELIC_SQL_OBFUSCATED;

  /* Set up the default datastore tracer configuration */
  config->datastore_tracer.instance_reporting = true;
  config->datastore_tracer.database_name_reporting = true;

  return config;
}

bool newrelic_destroy_app_config(newrelic_app_config_t** config) {
  if ((NULL == config) || (NULL == *config)) {
    return false;
  }

  nr_realfree((void**)config);

  return true;
}

nrtxnopt_t* newrelic_get_default_options(void) {
  nrtxnopt_t* opt = nr_zalloc(sizeof(nrtxnopt_t));

  opt->analytics_events_enabled = true;
  opt->custom_events_enabled = true;
  opt->synthetics_enabled = false;
  opt->instance_reporting_enabled = true;
  opt->database_name_reporting_enabled = true;
  opt->err_enabled = true;
  opt->request_params_enabled = false;
  opt->autorum_enabled = false;
  opt->error_events_enabled = true;
  opt->tt_enabled = true;
  opt->ep_enabled = false;
  opt->tt_recordsql = NR_SQL_OBFUSCATED;
  opt->tt_slowsql = true;
  opt->apdex_t = 0;
  opt->tt_threshold = 0;
  opt->ep_threshold = 500 * NR_TIME_DIVISOR_MS;
  opt->ss_threshold = 500 * NR_TIME_DIVISOR_MS;
  opt->cross_process_enabled = false;
  opt->tt_is_apdex_f = true;
  opt->allow_raw_exception_messages = true;
  opt->custom_parameters_enabled = true;
  opt->distributed_tracing_enabled = false;
  opt->span_events_enabled = false;

  return opt;
}

nrtxnopt_t* newrelic_get_transaction_options(
    const newrelic_app_config_t* config) {
  nrtxnopt_t* opt = newrelic_get_default_options();

  if (NULL != config) {
    /* Convert public datastore tracer settings to transaction options. */
    opt->instance_reporting_enabled
        = config->datastore_tracer.instance_reporting;
    opt->database_name_reporting_enabled
        = config->datastore_tracer.database_name_reporting;

    /* Convert public transaction tracer settings to transaction options. */
    opt->tt_enabled = (int)config->transaction_tracer.enabled;
    opt->tt_recordsql = newrelic_validate_recordsql(
        config->transaction_tracer.datastore_reporting.record_sql);
    opt->tt_slowsql = config->transaction_tracer.datastore_reporting.enabled;

    opt->ep_threshold
        = config->transaction_tracer.datastore_reporting.threshold_us
          * NR_TIME_DIVISOR_US;
    opt->ss_threshold = config->transaction_tracer.stack_trace_threshold_us
                        * NR_TIME_DIVISOR_US;

    if (NEWRELIC_THRESHOLD_IS_APDEX_FAILING
        == config->transaction_tracer.threshold) {
      opt->tt_is_apdex_f = 1;
      /* tt_threshold will be overwritten in nr_txn_begin() if tt_is_apdex_f is
       * set. */
    } else {
      opt->tt_is_apdex_f = 0;
      opt->tt_threshold = (uint64_t)config->transaction_tracer.duration_us;
    }
  }

  return opt;
}
