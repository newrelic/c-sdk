#include "tlib_php.h"

#include "php_agent.h"
#include "php_api_distributed_trace.h"

tlib_parallel_info_t parallel_info
    = {.suggested_nthreads = -1, .state_size = 0};

static void test_accept_distributed_trace_payload() {
  char* payload_string
      = "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"ac\":9123,\"ap\":51424,\"pa\":"
        "\"5fa3c01498e244a6\",\"id\":\"27856f70d3d314b7\",\"tr\":"
        "\"3221bf09aa0bcf0d\",\"pr\":0.1234,\"sa\":false,\"ti\":1482959525577}"
        "}";
  char* transport_type = "html";

  // test null transaction
  tlib_pass_if_false("NULL nr_php_api_accept_distributed_trace_payload",
                     nr_php_api_accept_distributed_trace_payload(
                         NULL, payload_string, transport_type),
                     "Expected null parameter to return false");
}

void test_main(void* p NRUNUSED) {
#if defined(ZTS) && !defined(PHP7)
  void*** tsrm_ls = NULL;
#endif /* ZTS && !PHP7 */

  tlib_php_engine_create("" PTSRMLS_CC);

  test_accept_distributed_trace_payload();

  tlib_php_engine_destroy(TSRMLS_C);
}
