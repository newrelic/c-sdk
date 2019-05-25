/*
 * This application creates a DT payload, writes it in a request file and
 * waits until a response files is created. The creation of a response
 * file indicates, that the request file was handled.
 *
 * The server handling the request is contained in
 * ex_distributed_tracing_server.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "libnewrelic.h"

#define REQUEST_FILE "/tmp/request"
#define RESPONSE_FILE "/tmp/response"

int main(void) {
  example_init();

  /* Initialize the config */
  char* app_name = get_app_name();
  if (NULL == app_name) {
    return -1;
  }

  char* license_key = get_license_key();
  if (NULL == license_key) {
    return -1;
  }

  newrelic_app_config_t* config
      = newrelic_create_app_config(app_name, license_key);

  customize_config(&config);

  /* Change the transaction tracer threshold to ensure a trace is generated */
  config->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  config->transaction_tracer.duration_us = 1;

  /* Enable distributed tracing */
  config->distributed_tracing.enabled = true;

  /* Wait up to 10 seconds for the SDK to connect to the daemon */
  newrelic_app_t* app = newrelic_create_app(config, 10000);
  newrelic_destroy_app_config(&config);

  /* Start a web transaction */
  newrelic_txn_t* txn = newrelic_start_web_transaction(app, "ExampleDTClient");

  /* Start an external segment */
  newrelic_segment_t* segment = newrelic_start_external_segment(
      txn, &(newrelic_external_segment_params_t){.procedure = "GET",
                                                 .uri = RESPONSE_FILE});

  /* Remove any left over request files */
  unlink(REQUEST_FILE);

  /*
   * Write a request file, which consists of a DT payload.
   *
   * NOTE: As a convention when communicating by HTTP/HTTPS, applications
   *       which are instrumented with New Relic send and receive DT
   *       payloads via the `newrelic` request header.
   */
  FILE* request_file = fopen(REQUEST_FILE, "w");

  if (request_file) {
    printf("Sending a request.\n");

    char* payload
        = newrelic_create_distributed_trace_payload_httpsafe(txn, segment);

    if (payload) {
      fprintf(request_file, "%s", payload);
      free(payload);
    }

    fclose(request_file);

    /* Wait for a response file to appear */
    printf("Waiting for response ... ");
    while (true) {
      FILE* response_file = fopen(RESPONSE_FILE, "r");

      if (response_file) {
        printf("received.\n\n");
        fclose(response_file);
        unlink(RESPONSE_FILE);
        break;
      }

      sleep(1);
    }
  } else {
    newrelic_notice_error(txn, 50, "Cannot send request",
                          "Error.class.request");
    printf("Error, cannot send request.\n");
  }

  /* End the external segment */
  newrelic_end_segment(txn, &segment);

  /* End web transaction */
  newrelic_end_transaction(&txn);

  newrelic_destroy_app(&app);

  return 0;
}
