/*
 * This application polls for a request file. If a request exists, its
 * contents are read and treated as base64 encoded distributed trace
 * payload. This payload is then accepted, the request file is deleted
 * and a response file is written.
 *
 * The client sending the request is contained in
 * ex_distributed_tracing_client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "libnewrelic.h"

#define REQUEST_FILE "/tmp/request"
#define RESPONSE_FILE "/tmp/response"

static char* read_file(FILE* fp) {
  char* buf = NULL;
  size_t len;

  if (fp) {
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = malloc(len + 1);
    if (buf) {
      buf[0] = '\0';
      fread(buf, 1, len, fp);
      buf[len] = '\0';
    }
  }

  return buf;
}

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

  printf("Server started, ready to accept requests.\n");

  /* Remove left over response files */
  unlink(RESPONSE_FILE);

  while (true) {
    /* Read and handle a request */
    FILE* request_file = fopen(REQUEST_FILE, "r");

    if (request_file) {
      printf("Request received.\n");

      /* Start a web transaction */
      newrelic_txn_t* txn
          = newrelic_start_web_transaction(app, "ExampleDTServer");

      sleep(1);

      /*
       * Read the payload from the request file and accept it
       *
       * NOTE: As a convention when communicating by HTTP/HTTPS, applications
       *       which are instrumented with New Relic send and receive DT
       *       payloads via the `newrelic` request header.
       */
      char* payload = read_file(request_file);

      if (payload) {
        printf("Accept payload.\n");

        newrelic_accept_distributed_trace_payload_httpsafe(
            txn, read_file(request_file), NEWRELIC_TRANSPORT_TYPE_OTHER);
        free(payload);
      }

      /* Close the request file */
      fclose(request_file);
      unlink(REQUEST_FILE);

      /* Write a response file to indicate that the request was handled */
      FILE* response_file = fopen(RESPONSE_FILE, "w");
      if (response_file) {
        printf("Writing response.\n\n");
        fclose(response_file);
      } else {
        newrelic_notice_error(txn, 50, "Cannot send response",
                              "Error.class.response");
        printf("Error writing response.\n\n");
      }

      /* End web transaction */
      newrelic_end_transaction(&txn);
    }
  }

  newrelic_destroy_app(&app);

  return 0;
}
