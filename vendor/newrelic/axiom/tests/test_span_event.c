#include "tlib_main.h"
#include "nr_span_event.h"
#include "util_memory.h"

static void test_span_event_create_destroy(void) {
  // create a few instances to make sure state stays separate
  // and destroy them to make sure any *alloc-y bugs are
  // caught by valgrind
  nr_span_event_t* ev;
  nr_span_event_t* null_ev = NULL;

  ev = nr_span_event_create();

  tlib_pass_if_not_null("create span events ev1", ev);

  nr_span_event_destroy(&ev);
  nr_span_event_destroy(&null_ev);
}

static void test_span_event_guid(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : should not set a null guid
  nr_span_event_set_guid(event, NULL);
  tlib_pass_if_null("NULL guid", nr_span_event_get_guid(event));

  // Test : should set the guid to an empty string
  nr_span_event_set_guid(event, "");
  tlib_pass_if_str_equal("empty string guid", "",
                         nr_span_event_get_guid(event));

  // Test : should set the guid
  nr_span_event_set_guid(event, "wombat");
  tlib_pass_if_str_equal("set the guid", "wombat",
                         nr_span_event_get_guid(event));

  // Test : One more set
  nr_span_event_set_guid(event, "Kangaroo");
  tlib_pass_if_str_equal("set a new guid", "Kangaroo",
                         nr_span_event_get_guid(event));

  nr_span_event_destroy(&event);
}

static void test_span_event_parent(void) {
  nr_span_event_t* event = nr_span_event_create();
  nr_span_event_t* parent_event = nr_span_event_create();
  nr_span_event_set_guid(parent_event, "wombat");

  // Test : that it does not blow up when we give a NULL pointer
  nr_span_event_set_parent(event, NULL);
  nr_span_event_set_parent(NULL, parent_event);
  tlib_pass_if_null("the parent should still be NULL",
                    nr_span_event_get_parent(event));

  // Test : the getter should return NULL when a NULL event is passed in
  tlib_pass_if_null("NULL event -> NULL parent",
                    nr_span_event_get_parent(NULL));

  // Test : that the parent is set correctly.
  nr_span_event_set_parent(event, parent_event);
  tlib_pass_if_ptr_equal("parent event should point to the one we set",
                         parent_event, nr_span_event_get_parent(event));
  tlib_pass_if_str_equal(
      "the parent guid should be the one we set earlier", "wombat",
      nr_span_event_get_guid(nr_span_event_get_parent(event)));

  nr_span_event_destroy(&event);
  nr_span_event_destroy(&parent_event);
}

static void test_span_event_transaction_id(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : that is does not blow up when we give the setter a NULL pointer
  nr_span_event_set_transaction_id(event, NULL);
  nr_span_event_set_transaction_id(NULL, "wallaby");
  tlib_pass_if_null("the transaction should still be NULL",
                    nr_span_event_get_transaction_id(event));

  // Test : the getter should not blow up when we send it an event with a NULL
  // transactionID
  tlib_pass_if_null("NULL event -> NULL transaction ID",
                    nr_span_event_get_transaction_id(event));

  // Test : setting the transaction id back and forth behaves as expected
  nr_span_event_set_transaction_id(event, "Florance");
  tlib_pass_if_str_equal("should be the transaction ID we set 1", "Florance",
                         nr_span_event_get_transaction_id(event));
  nr_span_event_set_transaction_id(event, "Wallaby");
  tlib_pass_if_str_equal("should be the transaction ID we set 2", "Wallaby",
                         nr_span_event_get_transaction_id(event));

  nr_span_event_destroy(&event);
}

static void test_span_event_name(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : that is does not blow up when we give the setter a NULL pointer
  nr_span_event_set_name(event, NULL);
  nr_span_event_set_name(NULL, "wallaby");
  tlib_pass_if_null("the name should still be NULL",
                    nr_span_event_get_name(event));

  // Test : the getter should not blow up when we send it an event with a NULL
  // name.
  tlib_pass_if_null("NULL event -> NULL name", nr_span_event_get_name(event));

  // Test : setting the name back and forth behaves as expected
  nr_span_event_set_name(event, "Florance");
  tlib_pass_if_str_equal("should be the name we set 1", "Florance",
                         nr_span_event_get_name(event));
  nr_span_event_set_name(event, "Wallaby");
  tlib_pass_if_str_equal("should be the name we set 2", "Wallaby",
                         nr_span_event_get_name(event));

  nr_span_event_destroy(&event);
}

static void test_span_event_category(void) {
  nr_span_event_t* event = nr_span_event_create();
  nr_span_category_t actual_value = nr_span_event_get_category(event);

  // Test : the default is NULL
  tlib_pass_if_true("The default category", NR_SPAN_GENERIC == actual_value,
                    "category=%d", (int)actual_value);
  actual_value = nr_span_event_get_category(NULL);
  tlib_pass_if_true("The default category, when NULL event given",
                    NR_SPAN_GENERIC == actual_value, "category=%d",
                    (int)actual_value);

  // Test : A null event returns NULL
  actual_value = nr_span_event_get_category(NULL);
  tlib_pass_if_true("NULL event -> default category",
                    NR_SPAN_GENERIC == actual_value, "category=%d",
                    (int)actual_value);

  // Test : passing a NULL event should not blow up
  nr_span_event_set_category(NULL, NR_SPAN_HTTP);

  // Test : setting the category back and forth
  nr_span_event_set_category(event, NR_SPAN_DATASTORE);
  actual_value = nr_span_event_get_category(event);
  tlib_pass_if_true("Category should be the one we set - datastore",
                    NR_SPAN_DATASTORE == actual_value, "category=%d",
                    (int)actual_value);
  nr_span_event_set_category(event, NR_SPAN_HTTP);
  actual_value = nr_span_event_get_category(event);
  tlib_pass_if_true("Category should be the one we set - http",
                    NR_SPAN_HTTP == actual_value, "category=%d",
                    (int)actual_value);

  nr_span_event_destroy(&event);
}

static void test_span_event_timestamp(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : Get timestamp with a NULL event
  tlib_pass_if_time_equal("NULL event should give zero", 0,
                          nr_span_event_get_timestamp(NULL));

  // Test : Set the timestamp a couple times
  nr_span_event_set_timestamp(event, 553483260);
  tlib_pass_if_time_equal("Get timestamp should equal 553483260", 553483260,
                          nr_span_event_get_timestamp(event));
  nr_span_event_set_timestamp(event, 853483260);
  tlib_pass_if_time_equal("Get timestamp should equal 853483260", 853483260,
                          nr_span_event_get_timestamp(event));

  nr_span_event_destroy(&event);
}

static void test_span_event_duration(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : get duration with a NULL event should return zero
  tlib_pass_if_time_equal("NULL event should give zero duration", 0,
                          nr_span_event_get_duration(NULL));

  // Test : Set duration a couple times
  nr_span_event_set_duration(event, 1);
  tlib_pass_if_time_equal("Get duration should be one", 1,
                          nr_span_event_get_duration(event));
  nr_span_event_set_duration(event, 341);
  tlib_pass_if_time_equal("Get duration should be one", 341,
                          nr_span_event_get_duration(event));

  nr_span_event_destroy(&event);
}

static void test_span_event_datastore_string_get_and_set(void) {
  nr_span_event_t* event = nr_span_event_create();

  // Test : that is does not blow up when we give the setter a NULL pointer
  nr_span_event_set_datastore(NULL, NR_SPAN_DATASTORE_COMPONENT, "wallaby");
  tlib_pass_if_null(
      "the component should still be NULL",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_COMPONENT));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_COMPONENT, NULL);
  tlib_pass_if_str_equal(
      "given a NULL value we should get an empty string", "",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_COMPONENT));

  // Test : the getter should not blow up when we send it an event with a NULL
  // component
  tlib_pass_if_null(
      "NULL event -> NULL component",
      nr_span_event_get_datastore(NULL, NR_SPAN_DATASTORE_COMPONENT));

  // Test : setting the component back and forth behaves as expected
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_COMPONENT, "chicken");
  tlib_pass_if_str_equal(
      "should be the component we set 1", "chicken",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_COMPONENT));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_COMPONENT, "oracle");
  tlib_pass_if_str_equal(
      "should be the component we set 2", "oracle",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_COMPONENT));

  // Test : setting and getting db_statement
  tlib_pass_if_null(
      "the db_statement should still be NULL",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_STATEMENT));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_DB_STATEMENT,
                              "SELECT * FROM BOBBY;");
  tlib_pass_if_str_equal(
      "set db_statement to BOBBY", "SELECT * FROM BOBBY;",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_STATEMENT));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_DB_STATEMENT,
                              "SELECT * FROM transactions;");
  tlib_pass_if_str_equal(
      "set db_statement to transactions", "SELECT * FROM transactions;",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_STATEMENT));

  // Test : setting and getting db_instance
  tlib_pass_if_null(
      "the db_statement should still be NULL",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_INSTANCE));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_DB_INSTANCE,
                              "I'm a box somewhere");
  tlib_pass_if_str_equal(
      "set db_statement to somewhere", "I'm a box somewhere",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_INSTANCE));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_DB_INSTANCE,
                              "some instance");
  tlib_pass_if_str_equal(
      "set db_statement to some instance", "some instance",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_DB_INSTANCE));

  // Test : setting and getting peer_addresss
  tlib_pass_if_null(
      "the db_statement should still be NULL",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_ADDRESS));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_PEER_ADDRESS,
                              "an address");
  tlib_pass_if_str_equal(
      "set db_statement to an address", "an address",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_ADDRESS));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_PEER_ADDRESS, "turkey");
  tlib_pass_if_str_equal(
      "set db_statement to turkey", "turkey",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_ADDRESS));

  // Test : setting and getting peer_hostname
  tlib_pass_if_null(
      "the db_statement should still be NULL",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_HOSTNAME));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_PEER_HOSTNAME, "wombat");
  tlib_pass_if_str_equal(
      "set db_statement to wombat", "wombat",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_HOSTNAME));
  nr_span_event_set_datastore(event, NR_SPAN_DATASTORE_PEER_HOSTNAME, "rabbit");
  tlib_pass_if_str_equal(
      "set db_statement to rabbit", "rabbit",
      nr_span_event_get_datastore(event, NR_SPAN_DATASTORE_PEER_HOSTNAME));

  nr_span_event_destroy(&event);
}

static void test_span_events_extern_get_and_set(void) {
  nr_span_event_t* span = nr_span_event_create();

  // Test : That nothing blows up if nulls are given.
  nr_span_event_set_external(NULL, NR_SPAN_EXTERNAL_URL, "no span");
  tlib_pass_if_null("The URL should still be NULL",
                    nr_span_event_get_external(span, NR_SPAN_EXTERNAL_URL));
  nr_span_event_set_external(span, NR_SPAN_EXTERNAL_COMPONENT, NULL);
  tlib_pass_if_str_equal(
      "When set external is given a NULL target value it should stay NULL",
      NULL, nr_span_event_get_external(span, NR_SPAN_EXTERNAL_COMPONENT));
  tlib_pass_if_null("NULL event -> NULL Method",
                    nr_span_event_get_external(span, NR_SPAN_EXTERNAL_METHOD));

  // Test : setting the component back and forth behaves as expected.
  nr_span_event_set_external(span, NR_SPAN_EXTERNAL_COMPONENT, "curl");
  tlib_pass_if_str_equal(
      "The component should be curl",
      nr_span_event_get_external(span, NR_SPAN_EXTERNAL_COMPONENT), "curl");
  nr_span_event_set_external(span, NR_SPAN_EXTERNAL_COMPONENT, "Guzzle 6");
  tlib_pass_if_str_equal(
      "The component should be Guzzle",
      nr_span_event_get_external(span, NR_SPAN_EXTERNAL_COMPONENT), "Guzzle 6");

  // Test : setting and getting the method and URL
  nr_span_event_set_external(span, NR_SPAN_EXTERNAL_METHOD, "GET");
  tlib_pass_if_str_equal(
      "The method should be GET",
      nr_span_event_get_external(span, NR_SPAN_EXTERNAL_METHOD), "GET");
  nr_span_event_set_external(span, NR_SPAN_EXTERNAL_URL, "wombats.com");
  tlib_pass_if_str_equal("The method should be wombats.com",
                         nr_span_event_get_external(span, NR_SPAN_EXTERNAL_URL),
                         "wombats.com");

  nr_span_event_destroy(&span);
}

tlib_parallel_info_t parallel_info = {.suggested_nthreads = 1, .state_size = 0};

void test_main(void* p NRUNUSED) {
  test_span_event_create_destroy();
  test_span_event_guid();
  test_span_event_parent();
  test_span_event_transaction_id();
  test_span_event_name();
  test_span_event_category();
  test_span_event_timestamp();
  test_span_event_duration();
  test_span_event_datastore_string_get_and_set();
  test_span_events_extern_get_and_set();
}
