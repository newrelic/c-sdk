#include "nr_axiom.h"
#include "tlib_main.h"
#include "nr_distributed_trace.h"
#include "nr_txn.h"
#include "nr_distributed_trace_private.h"
#include "util_memory.h"

static void test_distributed_trace_create_destroy(void) {
  // create a few instances to make sure state stays separate
  // and destroy them to make sure any *alloc-y bugs are
  // caught by valgrind
  nr_distributed_trace_t* dt1;
  nr_distributed_trace_t* dt2;
  nr_distributed_trace_t* null_dt = NULL;

  dt1 = nr_distributed_trace_create();
  dt2 = nr_distributed_trace_create();

  nr_distributed_trace_set_sampled(dt1, true);
  nr_distributed_trace_set_sampled(dt2, false);

  tlib_pass_if_true("Set sampled to true", nr_distributed_trace_is_sampled(dt1),
                    "Expected true, got false");

  tlib_pass_if_false("Set sampled to false",
                     nr_distributed_trace_is_sampled(dt2),
                     "Expected false, got true");

  nr_distributed_trace_destroy(&dt1);
  nr_distributed_trace_destroy(&dt2);
  nr_distributed_trace_destroy(&null_dt);
}

static void test_distributed_trace_field_account_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_get_account_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_get_account_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_app_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_get_app_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_get_app_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_guid(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_get_guid(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_get_guid(dt));

  /*
   * Test : Set value.
   */
  nr_distributed_trace_set_guid(dt, "guid");
  tlib_pass_if_str_equal("set guid", "guid", nr_distributed_trace_get_guid(dt));

  /*
   * Test : Unset value.
   */
  nr_distributed_trace_set_guid(dt, NULL);
  tlib_pass_if_null("unset guid", nr_distributed_trace_get_guid(dt));

  /*
   * Test : Changed value.
   */
  nr_distributed_trace_set_guid(dt, "a");
  nr_distributed_trace_set_guid(dt, "b");
  tlib_pass_if_str_equal("changed guid", "b",
                         nr_distributed_trace_get_guid(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_txn_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_get_txn_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_get_txn_id(dt));

  /*
   * Test : Set value.
   */
  nr_distributed_trace_set_txn_id(dt, "txn_id");
  tlib_pass_if_str_equal("set txn_id", "txn_id",
                         nr_distributed_trace_get_txn_id(dt));

  /*
   * Test : Unset value.
   */
  nr_distributed_trace_set_txn_id(dt, NULL);
  tlib_pass_if_null("unset txn_id", nr_distributed_trace_get_txn_id(dt));

  /*
   * Test : Changed value.
   */
  nr_distributed_trace_set_txn_id(dt, "a");
  nr_distributed_trace_set_txn_id(dt, "b");
  tlib_pass_if_str_equal("changed txn_id", "b",
                         nr_distributed_trace_get_txn_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_priority(void) {
  nr_distributed_trace_t dt = {.priority = 0.0};

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_double_equal("NULL dt", NR_PRIORITY_ERROR,
                            nr_distributed_trace_get_priority(NULL));

  /*
   * Test : Set value.
   */
  nr_distributed_trace_set_priority(&dt, 0.5);
  tlib_pass_if_double_equal("set priority", 0.5,
                            nr_distributed_trace_get_priority(&dt));

  /*
   * Test : Changed value.
   */
  nr_distributed_trace_set_priority(&dt, 0.8);
  tlib_pass_if_double_equal("set priority", 0.8,
                            nr_distributed_trace_get_priority(&dt));
}

static void test_distributed_trace_field_sampled(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  // test null cases
  tlib_pass_if_false("Return value for NULL pointer is false",
                     nr_distributed_trace_is_sampled(NULL),
                     "Expected false, got true");

  nr_distributed_trace_set_sampled(dt, true);
  nr_distributed_trace_set_sampled(NULL, false);

  tlib_pass_if_true("Value remains set after NULL pointer",
                    nr_distributed_trace_is_sampled(dt),
                    "Expected true, got false");

  // null case for destroy to make sure nothing explodes
  nr_distributed_trace_destroy(NULL);

  // test setting values back and forth
  nr_distributed_trace_set_sampled(dt, false);
  tlib_pass_if_false("Set sampled to false",
                     nr_distributed_trace_is_sampled(dt),
                     "Expected false, got true");

  nr_distributed_trace_set_sampled(dt, true);
  tlib_pass_if_true("Set sampled to true", nr_distributed_trace_is_sampled(dt),
                    "Expected true, got false");

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_trace_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_get_trace_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_get_trace_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_type(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_inbound_get_type(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_inbound_get_type(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_app_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_inbound_get_app_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value",
                    nr_distributed_trace_inbound_get_app_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_account_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt",
                    nr_distributed_trace_inbound_get_account_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value",
                    nr_distributed_trace_inbound_get_account_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_transport_type(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt",
                    nr_distributed_trace_inbound_get_transport_type(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value",
                    nr_distributed_trace_inbound_get_transport_type(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_timestamp_delta(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_long_equal(
      "NULL dt", 0, nr_distributed_trace_inbound_get_timestamp_delta(NULL, 0));

  /*
   * Test : Default value.
   */
  tlib_pass_if_long_equal(
      "default value", 0,
      nr_distributed_trace_inbound_get_timestamp_delta(dt, 0));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_guid(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_inbound_get_guid(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value", nr_distributed_trace_inbound_get_guid(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_field_inbound_txn_id(void) {
  nr_distributed_trace_t* dt;

  dt = nr_distributed_trace_create();

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL dt", nr_distributed_trace_inbound_get_txn_id(NULL));

  /*
   * Test : Default value.
   */
  tlib_pass_if_null("default value",
                    nr_distributed_trace_inbound_get_txn_id(dt));

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_payload_txn_payload_timestamp_delta(void) {
  nr_distributed_trace_t* dt;
  nrtime_t payload_timestamp_ms = 1529445826000;
  nrtime_t txn_timestamp_us = 15214458260000 * NR_TIME_DIVISOR_MS;
  nrtime_t delta_timestamp_us = nr_time_duration(
      txn_timestamp_us, (payload_timestamp_ms * NR_TIME_DIVISOR_MS));

  const char* error = NULL;
  nrobj_t* obj_payload;

  char* json
      = "{ \
    \"v\": [0,1],   \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1529445826000 \
    } \
  }";

  obj_payload = nro_create_from_json(json);

  dt = nr_distributed_trace_create();
  nr_distributed_trace_accept_inbound_payload(dt, obj_payload, "HTTP", &error);
  tlib_pass_if_null("No error", error);

  tlib_pass_if_long_equal(
      "Compare payload and txn time", delta_timestamp_us,
      nr_distributed_trace_inbound_get_timestamp_delta(dt, txn_timestamp_us));

  nr_distributed_trace_destroy(&dt);

  nro_delete(obj_payload);
}

static void test_distributed_trace_payload_create_destroy(void) {
  nr_distributed_trace_t* dt;

  nr_distributed_trace_payload_t* payload1;
  nr_distributed_trace_payload_t* payload2;
  nr_distributed_trace_payload_t* null_payload = NULL;

  dt = nr_distributed_trace_create();

  payload1 = nr_distributed_trace_payload_create(NULL, "1234");
  payload2 = nr_distributed_trace_payload_create(dt, NULL);

  // null case for destroy to make sure nothing explodes
  nr_distributed_trace_payload_destroy(NULL);
  nr_distributed_trace_payload_destroy(&null_payload);

  tlib_pass_if_true(
      "parent_id is set correctly",
      nr_strcmp(nr_distributed_trace_payload_get_parent_id(payload1), "1234")
          == 0,
      "Expected true, got false");

  tlib_pass_if_true("Distributed metadata is set correctly",
                    dt == nr_distributed_trace_payload_get_metadata(payload2),
                    "Expected true, got false");

  nr_distributed_trace_payload_destroy(&payload1);
  nr_distributed_trace_payload_destroy(&payload2);

  nr_distributed_trace_destroy(&dt);
}

static void test_distributed_trace_convert_payload_to_object(void) {
  const char* error = NULL;
  char* json = NULL;
  nrobj_t* payload = NULL;
  char* payload_string;

  // NULL Payload
  nr_distributed_trace_convert_payload_to_object(NULL, &error);
  tlib_pass_if_str_equal(
      "Empty DT", "Supportability/DistributedTrace/AcceptPayload/Ignored/Null",
      error);

  // Non-null error passed in (make sure it doesn't get overridden)
  error = "ZipZap";
  nr_distributed_trace_convert_payload_to_object(NULL, &error);
  tlib_pass_if_str_equal("Non-null Error", "ZipZap", error);
  error = NULL;

  // Invalid JSON
  nr_distributed_trace_convert_payload_to_object("Invalid json", &error);
  tlib_pass_if_str_equal(
      "Invalid payload",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  error = NULL;

  // Missing version
  json = nr_strdup(
      "{ \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1482959525577 \
    } \
  }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing version",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Incompatible major version
  json = nr_strdup(
      "{ \
    \"v\": [1,1],   \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1482959525577 \
    } \
  }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Major version too high",
      "Supportability/DistributedTrace/AcceptPayload/Ignored/MajorVersion",
      error);
  nr_free(json);
  error = NULL;

  // Missing required key: Type
  json = nr_strdup(
      "{ \
    \"v\": [0,1],   \
    \"d\": {        \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1482959525577 \
    } \
  }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing required key: Type",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Missing required key: Account ID
  json = nr_strdup(
      "{ \
    \"v\": [0,9],   \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1482959525577 \
    } \
  }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing required key: Account ID",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Missing required key: Application ID
  json = nr_strdup(
      "{ \
      \"v\": [0,1],   \
      \"d\": {        \
        \"ty\": \"App\", \
        \"ac\": \"9123\", \
        \"id\": \"27856f70d3d314b7\", \
        \"tr\": \"3221bf09aa0bcf0d\", \
        \"pr\": 0.1234, \
        \"sa\": false, \
        \"ti\": 1482959525577 \
      } \
    }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing required key: Application ID",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Missing BOTH txn_id AND guid
  json = nr_strdup(
      "{ \
      \"v\": [0,1],   \
      \"d\": {        \
        \"ty\": \"App\", \
        \"ac\": \"9123\", \
        \"ap\": \"51424\", \
        \"tr\": \"3221bf09aa0bcf0d\", \
        \"pr\": 0.1234, \
        \"sa\": true, \
        \"ti\": 1482959525577 \
      } \
    }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Inbound distributed trace must have either d.tx or d.id, missing both",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Missing txn_id, guid present
  json = nr_strdup(
      "{"
      "\"v\":[0,1],"
      "\"d\":{"
      "\"ty\":\"App\","
      "\"ac\":\"9123\","
      "\"ap\":\"51424\","
      "\"id\":\"14a8b295952a55f7\","
      "\"tr\":\"3221bf09aa0bcf0d\","
      "\"pr\":0.12340,"
      "\"sa\":true,"
      "\"ti\":1482959525577"
      "}"
      "}");
  payload = nr_distributed_trace_convert_payload_to_object(json, &error);
  payload_string = nro_to_json(payload);
  tlib_pass_if_null("there should not be errors", error);
  tlib_pass_if_not_null("the payload should not be null", payload);
  tlib_pass_if_str_equal("The payload object should equal the json object",
                         json, payload_string);
  nr_free(json);
  nr_free(payload_string);
  nro_real_delete(&payload);
  error = NULL;

  // txn_id present, Missing guid
  json = nr_strdup(
      "{"
      "\"v\":[0,1],"
      "\"d\":{"
      "\"ty\":\"App\","
      "\"ac\":\"9123\","
      "\"ap\":\"51424\","
      "\"tr\":\"3221bf09aa0bcf0d\","
      "\"pr\":0.12340,"
      "\"sa\":true,"
      "\"ti\":1482959525577,"
      "\"tx\":\"14a8b295952a55f7\""
      "}"
      "}");
  payload = nr_distributed_trace_convert_payload_to_object(json, &error);
  payload_string = nro_to_json(payload);
  tlib_pass_if_null("there should not be errors", error);
  tlib_pass_if_not_null("the payload should not be null", payload);
  tlib_pass_if_str_equal("The payload object should equal the json object",
                         json, payload_string);
  nr_free(json);
  nr_free(payload_string);
  nro_real_delete(&payload);
  error = NULL;

  // Missing required key: Trace ID
  json = nr_strdup(
      "{ \
      \"v\": [0,1],   \
      \"d\": {        \
        \"ty\": \"App\", \
        \"ac\": \"9123\", \
        \"ap\": \"51424\", \
        \"id\": \"27856f70d3d314b7\", \
        \"pr\": 0.1234, \
        \"sa\": false, \
        \"ti\": 1482959525577 \
      } \
    }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing required key: Trace ID",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
  error = NULL;

  // Missing required key: Timestamp
  json = nr_strdup(
      "{ \
    \"v\": [0,1],   \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"pr\": 0.1234, \
      \"sa\": false \
    } \
  }");
  nr_distributed_trace_convert_payload_to_object(json, &error);
  tlib_pass_if_str_equal(
      "Missing required key: Timestamp",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  nr_free(json);
}

static void test_distributed_trace_payload_accept_inbound_payload(void) {
  nr_distributed_trace_t* dt;
  nrtime_t payload_timestamp_ms = 1482959525577;
  nrtime_t txn_timestamp_us
      = (payload_timestamp_ms * NR_TIME_DIVISOR_MS) - 100000000;

  const char* error = NULL;
  nrobj_t* obj_payload;

  char* json
      = "{ \
    \"v\": [0,1],   \
    \"d\": {        \
      \"ty\": \"App\", \
      \"ac\": \"9123\", \
      \"ap\": \"51424\", \
      \"id\": \"27856f70d3d314b7\", \
      \"tr\": \"3221bf09aa0bcf0d\", \
      \"tx\": \"6789\", \
      \"id\": \"4321\", \
      \"tk\": \"1010\", \
      \"pr\": 0.1234, \
      \"sa\": false, \
      \"ti\": 1482959525577 \
    } \
  }";

  obj_payload = nro_create_from_json(json);

  dt = nr_distributed_trace_create();

  /*
   * Test : Null payload
   */
  tlib_pass_if_false(
      "Null payload",
      nr_distributed_trace_accept_inbound_payload(dt, NULL, "", &error),
      "Expected false");
  tlib_pass_if_str_equal(
      "Null payload",
      "Supportability/DistributedTrace/AcceptPayload/ParseException", error);
  error = NULL;

  /*
   * Test : Null DT
   */
  tlib_pass_if_false("Null dt",
                     nr_distributed_trace_accept_inbound_payload(
                         NULL, obj_payload, "", &error),
                     "Expected false");
  tlib_pass_if_str_equal(
      "Null dt", "Supportability/DistributedTrace/AcceptPayload/Exception",
      error);
  error = NULL;

  /*
   * Test : Successful
   */
  tlib_pass_if_true("Inbound processed",
                    nr_distributed_trace_accept_inbound_payload(
                        dt, obj_payload, "Other", &error),
                    "Expected NULL");
  tlib_pass_if_null("No supportability metric error thrown", error);
  tlib_pass_if_str_equal("Type", "App",
                         nr_distributed_trace_inbound_get_type(dt));
  tlib_pass_if_str_equal("Application ID", "51424",
                         nr_distributed_trace_inbound_get_app_id(dt));
  tlib_pass_if_str_equal("Account ID", "9123",
                         nr_distributed_trace_inbound_get_account_id(dt));
  tlib_pass_if_str_equal("Event Parent", "4321",
                         nr_distributed_trace_inbound_get_guid(dt));
  tlib_pass_if_str_equal("Transaction ID", "6789",
                         nr_distributed_trace_inbound_get_txn_id(dt));
  tlib_pass_if_str_equal("Transport Type", "Other",
                         nr_distributed_trace_inbound_get_transport_type(dt));
  tlib_pass_if_uint_equal(
      "Timestamp",
      nr_time_duration(txn_timestamp_us,
                       (payload_timestamp_ms * NR_TIME_DIVISOR_MS)),
      nr_distributed_trace_inbound_get_timestamp_delta(dt, txn_timestamp_us));

  nr_distributed_trace_destroy(&dt);
  nro_delete(obj_payload);
}

static void test_distributed_trace_payload_as_text(void) {
  nr_distributed_trace_t dt = {.priority = 0.5};
  nr_distributed_trace_payload_t payload
      = {.metadata = NULL, .parent_id = NULL, .timestamp = 60000};
  char* text;

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("NULL payload", nr_distributed_trace_payload_as_text(NULL));
  tlib_pass_if_null("NULL trace",
                    nr_distributed_trace_payload_as_text(&payload));

  /*
   * Test : Missing id and transaction id.
   */
  payload.metadata = &dt;
  tlib_pass_if_null("NULL payload", nr_distributed_trace_payload_as_text(NULL));
  tlib_pass_if_null("NULL trace",
                    nr_distributed_trace_payload_as_text(&payload));

  /*
   * Test : Valid payload, with all nullable fields NULL.
   */
  payload.metadata = &dt;
  dt.txn_id = "txnid";
  text = nr_distributed_trace_payload_as_text(&payload);
  tlib_pass_if_str_equal("NULL fields",
                         "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"tx\":\"txnid\","
                         "\"pr\":0.50000,\"sa\":false,\"ti\":60}}",
                         text);
  nr_free(text);

  /*
   * Test : Valid payload, with all nullable fields NULL.
   */
  payload.metadata = &dt;
  dt.txn_id = NULL;
  dt.guid = "guid";
  text = nr_distributed_trace_payload_as_text(&payload);
  tlib_pass_if_str_equal("NULL fields",
                         "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"id\":\"guid\","
                         "\"pr\":0.50000,\"sa\":false,\"ti\":60}}",
                         text);
  nr_free(text);

  /*
   * Test : Valid payload, with all fields set.
   */
  dt.account_id = "account";
  dt.app_id = "app";
  dt.trace_id = "trace";
  dt.trusted_key = "tkey";
  dt.txn_id = "txnid";
  text = nr_distributed_trace_payload_as_text(&payload);
  tlib_pass_if_str_equal(
      "set fields",
      "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"ac\":\"account\",\"ap\":\"app\","
      "\"id\":\"guid\",\"tr\":\"trace\",\"tx\":\"txnid\",\"pr\":0.50000,"
      "\"sa\":false,\"ti\":60,\"tk\":\"tkey\"}}",
      text);
  nr_free(text);

  /*
   * Test : Valid payload, trusted key matches account id
   */
  dt.account_id = "account";
  dt.app_id = "app";
  dt.trace_id = "trace";
  dt.trusted_key = "account";
  text = nr_distributed_trace_payload_as_text(&payload);
  tlib_pass_if_str_equal(
      "set fields",
      "{\"v\":[0,1],\"d\":{\"ty\":\"App\",\"ac\":\"account\",\"ap\":\"app\","
      "\"id\":\"guid\",\"tr\":\"trace\",\"tx\":\"txnid\",\"pr\":0.50000,"
      "\"sa\":false,\"ti\":60}}",
      text);
  nr_free(text);
}

tlib_parallel_info_t parallel_info = {.suggested_nthreads = 2, .state_size = 0};

void test_main(void* p NRUNUSED) {
  test_distributed_trace_create_destroy();
  test_distributed_trace_field_account_id();
  test_distributed_trace_field_app_id();
  test_distributed_trace_field_guid();
  test_distributed_trace_field_txn_id();
  test_distributed_trace_field_priority();
  test_distributed_trace_field_sampled();
  test_distributed_trace_field_trace_id();

  test_distributed_trace_field_inbound_type();
  test_distributed_trace_field_inbound_app_id();
  test_distributed_trace_field_inbound_account_id();
  test_distributed_trace_field_inbound_transport_type();
  test_distributed_trace_field_inbound_timestamp_delta();
  test_distributed_trace_field_inbound_guid();
  test_distributed_trace_field_inbound_txn_id();

  test_distributed_trace_payload_txn_payload_timestamp_delta();

  test_distributed_trace_payload_create_destroy();
  test_distributed_trace_convert_payload_to_object();
  test_distributed_trace_payload_accept_inbound_payload();
  test_distributed_trace_payload_as_text();
}
