/*DESCRIPTION
Record custom events in different threads.
*/

/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/

// clang-format off
/*EXPECT_CUSTOM_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": 10000
  },
  "?? events"
]
*/
// clang-format on

#include "common.h"
#include <unistd.h>
#include <pthread.h>

static void* create_event(void* argp) {
  newrelic_txn_t* txn = (newrelic_txn_t*)argp;
  newrelic_custom_event_t* event;

  event = newrelic_create_custom_event("Some Name");
  newrelic_custom_event_add_attribute_string(event, "a-string", "Forty Two");

  newrelic_record_custom_event(txn, &event);

  return NULL;
}

RUN_NONWEB_TXN("basic") {
  pthread_t thread_ids[10000];

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_create(&thread_ids[i], NULL, create_event, (void*)txn);
  }

  for (size_t i = 0; i < sizeof(thread_ids) / sizeof(thread_ids[0]); i++) {
    pthread_join(thread_ids[i], NULL);
  }
}
