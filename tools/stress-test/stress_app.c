#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <time.h>

#include "stress_app.h"
#include "libnewrelic.h"
#include "util_memory.h"

newrelic_app_t* newrelic_get_app(void) {
  newrelic_app_t* app = 0;
  newrelic_app_config_t* config = 0;

  newrelic_configure_log("./c_sdk.log", NEWRELIC_LOG_INFO);

  /* Staging account 432507 */
  config = newrelic_create_app_config(
      "C-Agent Test App", "07a2ad66c637a29c3982469a3fe8d1982d002c4a");
  strcpy(config->redirect_collector, "staging-collector.newrelic.com");

  /* Wait up to 10 seconds for the agent to connect to the daemon */
  app = newrelic_create_app(config, 10000);
  newrelic_destroy_app_config(&config);
  return app;
}

void do_transaction_and_error(newrelic_app_t* app,
                              const char* transaction_name,
                              const char* error_name) {
  newrelic_txn_t* txn = 0;
  txn = newrelic_start_non_web_transaction(app, transaction_name);
  newrelic_notice_error(txn, 50, "Error from the stress app.", error_name);
  newrelic_end_transaction(&txn);
}

int main(int argc, char** argv) {
  newrelic_app_t* app = 0;

  const char transaction_name[] = "StressTransaction_1";
  const char error_name[] = "StressTransactionError";

  // date formats, pointed at because that's what strftime wants
  char format_start_array[] = "AAAA-BB-CC DD:EE:FF";
  char* formatted_start = format_start_array;

  char formated_now_array[] = "AAAA-BB-CC DD:EE:FF";
  char* formated_now = formated_now_array;

  time_t raw_start_time;
  struct tm* start_timeinfo;

  time_t raw_current_time;
  struct tm* current_timeinfo;

  int total_errors_sent = 0;
  int num_transaction = 10;
  int per_seconds = 6;
  int seconds_to_run = 60 * 10;

  // ensure we have three arguments
  if (4 != argc) {
    (void)(argv);
    printf(
        "USAGE: ./stress_app number_of_transaction per_seconds "
        "for_n_seconds\n\n");
    exit(1);
  }

  num_transaction = atoi(argv[1]);
  per_seconds = atoi(argv[2]);
  seconds_to_run = atoi(argv[3]);

  if (((1 > num_transaction) || (1 > per_seconds) || (1 > seconds_to_run))) {
    fprintf(stderr, "ERROR: Arguments should be positive integers.\n");
    exit(1);
  }

  // make our intention clear
  printf(
      "I will send %i transactions every %i seconds, for %i seconds total.\n\n",
      num_transaction, per_seconds, seconds_to_run);

  app = newrelic_get_app();

  // capture start time *after* the app connects
  time(&raw_start_time);
  start_timeinfo = localtime(&raw_start_time);
  strftime(formatted_start, 20, "%Y-%m-%d %H:%M:%S", start_timeinfo);

  // second variable for current time
  time(&raw_current_time);

  while (true) {
    for (short i = 0; i < num_transaction; i++) {
      do_transaction_and_error(app, transaction_name, error_name);
      total_errors_sent++;
    }
    printf("I have sent %i transactions/errors since: %s\n", total_errors_sent,
           formatted_start);
    printf("I have been running for %lld seconds. \n\n",
           (long long)raw_current_time - (long long)raw_start_time);
    sleep(per_seconds);

    time(&raw_current_time);
    if ((long long)raw_current_time - (long long)raw_start_time
        >= seconds_to_run) {
      break;
    }

    current_timeinfo = localtime(&raw_current_time);
    strftime(formated_now, 32, "%Y-%m-%d %H:%M:%S", current_timeinfo);
    printf("It is now %s\n", formated_now);
  }

  newrelic_destroy_app(&app);

  current_timeinfo = localtime(&raw_current_time);
  strftime(formated_now, 32, "%Y-%m-%d %H:%M:%S", current_timeinfo);
  printf("I finished running at %s\n\n", formated_now);
  printf("I sent %i transactions in total\n\n", total_errors_sent);
  printf("Validation Commands\n");
  printf("--------------------------------------------------\n");
  printf("php tools/stress-test/test.php \"%s\" \"%s\" \n", formatted_start,
         formated_now);
  printf("docker run -it c-agent-error-validation \"%s\" \"%s\" \n",
         formatted_start, formated_now);

  return 0;
}
