#include "nr_axiom.h"
#include "nr_agent.h"

#include "tlib_main.h"

static void test_conn_params_init(void) {
  nr_conn_params_t* params;
  /*
   * Test : Bad parameters
   */
  params = nr_conn_params_init(NULL);
  tlib_pass_if_int_equal("Supplying a NULL path yields unknown connection type",
                         params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init("1234567890");
  tlib_pass_if_int_equal(
      "Supplying a too-big port yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init("host:1234567890");
  tlib_pass_if_int_equal(
      "Supplying a too-big port yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init("-1");
  tlib_pass_if_int_equal(
      "Supplying a too-small port yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init("../not/absolute.txt");
  tlib_pass_if_int_equal(
      "Supplying a relative path yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init(
      "/this/is/a/very/long/absolute/path/this/is/a/very/"
      "long/absolute/path/this/is/a/very/long/absolute/"
      "path/this/is/a/very/long/absolute/path/this/is/a/"
      "very/long/absolute/path/absolute.txt");
  tlib_pass_if_int_equal(
      "Supplying a too-long path yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init("127.0.0.1:");
  tlib_pass_if_int_equal(
      "Supplying a only a host: yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init(":9000");
  tlib_pass_if_int_equal(
      "Supplying a only a :port yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

  params = nr_conn_params_init(":90x");
  tlib_pass_if_int_equal(
      "Supplying an invalid :port yields an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);

#if NR_SYSTEM_LINUX
  params = nr_conn_params_init("@");
  tlib_pass_if_int_equal(
      "Supplying only an at must yield an unknown connection type",
      params->type, NR_AGENT_CONN_UNKNOWN);
  nr_conn_params_free(params);
#endif

  /*
   * Test : Well-formed inputs
   */
  params = nr_conn_params_init("1");
  tlib_pass_if_int_equal(
      "Supplying a well-formed port must yield a loopback connection type",
      params->type, NR_AGENT_CONN_TCP_LOOPBACK);
  tlib_pass_if_int_equal("Supplying a well-formed port must yield a port field",
                         1, params->location.port);
  nr_conn_params_free(params);

  params = nr_conn_params_init("/this/is/absolute.txt");
  tlib_pass_if_int_equal(
      "Supplying an absolute path yields an unix domain socket connection type",
      params->type, NR_AGENT_CONN_UNIX_DOMAIN_SOCKET);
  tlib_pass_if_str_equal(
      "Supplying an absolute path must yield a udspath field",
      "/this/is/absolute.txt", params->location.udspath);
  nr_conn_params_free(params);

#if NR_SYSTEM_LINUX
  params = nr_conn_params_init("@newrelic");
  tlib_pass_if_int_equal(
      "Supplying an atted path must yield a abstract socket connection type",
      params->type, NR_AGENT_CONN_ABSTRACT_SOCKET);
  tlib_pass_if_str_equal("Supplying an atted path must yield a udspath field",
                         "@newrelic", params->location.udspath);
  nr_conn_params_free(params);

  params = nr_conn_params_init("@/path/to/newrelic");
  tlib_pass_if_int_equal(
      "Supplying an atted path must yield a abstract socket connection type",
      params->type, NR_AGENT_CONN_ABSTRACT_SOCKET);
  tlib_pass_if_str_equal("Supplying an atted path must yield a udspath field",
                         "@/path/to/newrelic", params->location.udspath);
  nr_conn_params_free(params);
#endif

  params = nr_conn_params_init("127.0.0.1:9000");
  tlib_pass_if_int_equal(
      "Supplying a host:port must yield a TCP host + port connection type",
      params->type, NR_AGENT_CONN_TCP_HOST_PORT);
  tlib_pass_if_str_equal("Supplying host:port must yield an address field",
                         "127.0.0.1", params->location.address.host);
  tlib_pass_if_int_equal("Supplying host:port must yield a port field", 9000,
                         params->location.address.port);
  nr_conn_params_free(params);

  params = nr_conn_params_init("[2001:2001:2001:11]:9000");
  tlib_pass_if_int_equal(
      "Supplying a host:port must yield a TCP host + port connection type",
      params->type, NR_AGENT_CONN_TCP_HOST_PORT);
  tlib_pass_if_str_equal("Supplying host:port must yield an address field",
                         "2001:2001:2001:11", params->location.address.host);
  tlib_pass_if_int_equal("Supplying host:port must yield a port field", 9000,
                         params->location.address.port);
  nr_conn_params_free(params);
}

tlib_parallel_info_t parallel_info = {.suggested_nthreads = 2, .state_size = 0};

void test_main(void* p NRUNUSED) {
  test_conn_params_init();
}
