#include "nr_axiom.h"

#include "nr_exclusive_time.h"
#include "nr_exclusive_time_private.h"
#include "util_memory.h"
#include "util_time.h"
#include "util_vector.h"

#include "tlib_main.h"

static void test_create_destroy(void) {
  nr_exclusive_time_t* et = NULL;

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_null("start time after stop time should fail to create",
                    nr_exclusive_time_create(2, 1));
  tlib_pass_if_bool_equal("NULL exclusive time pointer should fail to destroy",
                          false, nr_exclusive_time_destroy(NULL));
  tlib_pass_if_bool_equal("NULL exclusive time should fail to destroy", false,
                          nr_exclusive_time_destroy(&et));

  /*
   * Test : Normal operation.
   */
  et = nr_exclusive_time_create(1, 2);
  tlib_pass_if_not_null("create should succeed", et);
  tlib_pass_if_time_equal("create should set the start time", 1,
                          et->start_time);
  tlib_pass_if_time_equal("create should set the stop time", 2, et->stop_time);
  tlib_pass_if_size_t_equal(
      "create should createialise an empty transitions vector", 0,
      nr_vector_size(&et->transitions));

  tlib_pass_if_bool_equal("destroy should succeed", true,
                          nr_exclusive_time_destroy(&et));
  tlib_pass_if_null("destroy should NULL out the pointer", et);
}

static void test_add_child(void) {
  nr_exclusive_time_t* et;
  nr_exclusive_time_transition_t* trans;

  et = nr_exclusive_time_create(1, 4);

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_bool_equal("a child cannot be added to a NULL exclusive time",
                          false, nr_exclusive_time_add_child(NULL, 1, 2));
  tlib_pass_if_bool_equal(
      "a child cannot be added with a start time after its stop time", false,
      nr_exclusive_time_add_child(et, 2, 1));

  /*
   * Test : Normal operation.
   */
  tlib_pass_if_bool_equal(
      "adding a child completely within the bounds of the parent should "
      "succeed",
      true, nr_exclusive_time_add_child(et, 2, 3));
  tlib_pass_if_size_t_equal("adding a normal child should add two transitions",
                            2, nr_vector_size(&et->transitions));

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 0);
  tlib_pass_if_int_equal(
      "the first added transition should have the start type", (int)CHILD_START,
      (int)trans->type);
  tlib_pass_if_time_equal("the first added transition should be at time 2", 2,
                          trans->time);

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 1);
  tlib_pass_if_int_equal(
      "the second added transition should have the stop type", (int)CHILD_STOP,
      (int)trans->type);
  tlib_pass_if_time_equal("the second added transition should be at time 3", 3,
                          trans->time);

  tlib_pass_if_bool_equal(
      "adding a child with the exact bounds of the parent should succeed", true,
      nr_exclusive_time_add_child(et, 1, 4));
  tlib_pass_if_size_t_equal("adding a normal child should add two transitions",
                            4, nr_vector_size(&et->transitions));

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 2);
  tlib_pass_if_int_equal(
      "the first added transition should have the start type", (int)CHILD_START,
      (int)trans->type);
  tlib_pass_if_time_equal("the first added transition should be at time 2", 1,
                          trans->time);

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 3);
  tlib_pass_if_int_equal(
      "the second added transition should have the stop type", (int)CHILD_STOP,
      (int)trans->type);
  tlib_pass_if_time_equal("the second added transition should be at time 3", 4,
                          trans->time);

  tlib_pass_if_bool_equal(
      "adding a child with the same start and stop time should succeed", true,
      nr_exclusive_time_add_child(et, 1, 1));
  tlib_pass_if_size_t_equal("adding a normal child should add two transitions",
                            6, nr_vector_size(&et->transitions));

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 4);
  tlib_pass_if_int_equal(
      "the first added transition should have the start type", (int)CHILD_START,
      (int)trans->type);
  tlib_pass_if_time_equal("the first added transition should be at time 2", 1,
                          trans->time);

  trans = (nr_exclusive_time_transition_t*)nr_vector_get(&et->transitions, 5);
  tlib_pass_if_int_equal(
      "the second added transition should have the stop type", (int)CHILD_STOP,
      (int)trans->type);
  tlib_pass_if_time_equal("the second added transition should be at time 3", 1,
                          trans->time);

  tlib_pass_if_bool_equal(
      "adding a child before the parent should succeed, but do nothing", true,
      nr_exclusive_time_add_child(et, 0, 0));
  tlib_pass_if_size_t_equal(
      "adding an abnormal child should leave the transitions vector alone", 6,
      nr_vector_size(&et->transitions));

  tlib_pass_if_bool_equal(
      "adding a child after the parent should succeed, but do nothing", true,
      nr_exclusive_time_add_child(et, 5, 5));
  tlib_pass_if_size_t_equal(
      "adding an abnormal child should leave the transitions vector alone", 6,
      nr_vector_size(&et->transitions));

  nr_exclusive_time_destroy(&et);
}

static void test_calculate(void) {
  nr_exclusive_time_t* et;
  nr_exclusive_time_transition_t* trans;

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_time_equal("NULL exclusive time should fail", false,
                          nr_exclusive_time_calculate(NULL));

  /*
   * Test : Empty exclusive time.
   */
  et = nr_exclusive_time_create(10, 50);

  tlib_pass_if_time_equal(
      "a segment with no children should have its entire duration attributed "
      "as exclusive time",
      40, nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : Typical synchronous operation: two children, each of 10 us, not
   *        overlapping.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *                     Child----->
   *                                    Child----->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 20, 30);
  nr_exclusive_time_add_child(et, 35, 45);

  tlib_pass_if_time_equal("synchronous children", 20,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : Typical synchronous operation: two children, each of 10 us, not
   *        overlapping, but touching.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *                     Child----->
   *                               Child----->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 20, 30);
  nr_exclusive_time_add_child(et, 30, 40);

  tlib_pass_if_time_equal("synchronous children with separation anxiety", 20,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : All children are zero duration.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *                     C
   *                               C
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 20, 20);
  nr_exclusive_time_add_child(et, 30, 30);

  tlib_pass_if_time_equal("wee bairns", 40, nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : Overlapping asynchronous children: two children, each of 10 us, but
   *        overlapping for 5 us.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *                     Child----->
   *                          Child----->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 20, 30);
  nr_exclusive_time_add_child(et, 25, 35);

  tlib_pass_if_time_equal("asynchronous children", 25,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : Overlapping asynchronous children, but partly outside of the
   *        parent segment: two children, each of 10 us, but overlapping for 5
   *        us, and with only 10 us within the bounds of the parent segment.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *      Child----->
   *           Child----->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 5, 15);
  nr_exclusive_time_add_child(et, 10, 20);

  tlib_pass_if_time_equal("asynchronous children who have partially left home",
                          30, nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : Overlapping asynchronous children, but partly outside of the
   *        parent segment: two children, each of 10 us, but overlapping for 5
   *        us, and with only 10 us within the bounds of the parent segment.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *                                              Child----->
   *           Child----->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 45, 55);
  nr_exclusive_time_add_child(et, 10, 20);

  tlib_pass_if_time_equal("asynchronous children who have partially left home",
                          25, nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : A child that starts before and ends after its parent.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   *      Child--------------------------------------------->
   */
  et = nr_exclusive_time_create(10, 50);
  nr_exclusive_time_add_child(et, 5, 55);

  tlib_pass_if_time_equal("time travelling, long lived children", 0,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : No children within the bounds of the segment. This can't happen in
   *        normal operation with nr_exclusive_time_add_child(), but testing
   *        this ensures the robustness of the walking algorithm.
   *
   * time ->   10        20        30        40        50
   *           Parent---------------------------------->
   * Child>
   *                                                        Child----->
   */
  et = nr_exclusive_time_create(10, 50);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_START;
  trans->time = 0;
  nr_vector_push_back(&et->transitions, trans);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_STOP;
  trans->time = 5;
  nr_vector_push_back(&et->transitions, trans);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_START;
  trans->time = 55;
  nr_vector_push_back(&et->transitions, trans);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_STOP;
  trans->time = 65;
  nr_vector_push_back(&et->transitions, trans);

  tlib_pass_if_time_equal("wayward children", 40,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : One CHILD_START only, which should be effectively ignored.
   */
  et = nr_exclusive_time_create(10, 50);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_START;
  trans->time = 20;
  nr_vector_push_back(&et->transitions, trans);

  tlib_pass_if_time_equal("mismatched CHILD_START", 40,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : One CHILD_STOP only, which should be effectively ignored.
   */
  et = nr_exclusive_time_create(10, 50);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  trans->type = CHILD_STOP;
  trans->time = 20;
  nr_vector_push_back(&et->transitions, trans);

  tlib_pass_if_time_equal("mismatched CHILD_STOP", 40,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);

  /*
   * Test : A transition with an unknown type should be ignored.
   *
   *        This test technically uses undefined behaviour in C in assuming
   *        that 0xFFFFFFFF or 0xFFFFFFFFFFFFFFFF (depending on word size) is
   *        not a valid value within the enum. If we get weird behaviour when
   *        we port this to some weird mainframe, it's probably that.
   */
  et = nr_exclusive_time_create(10, 50);

  trans = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  nr_memset(&trans->type, 0xff, sizeof(trans->type));
  trans->time = 20;
  nr_vector_push_back(&et->transitions, trans);

  tlib_pass_if_time_equal("not a child at all; maybe a dog", 40,
                          nr_exclusive_time_calculate(et));

  nr_exclusive_time_destroy(&et);
}

static void test_compare(void) {
  nr_exclusive_time_transition_t a = {.time = 0, .type = CHILD_START};
  nr_exclusive_time_transition_t b = {.time = 0, .type = CHILD_START};

  /*
   * Test : Bad parameters.
   */
  tlib_pass_if_int_equal(
      "a NULL pointer is always considered equal to its counterpart", 0,
      nr_exclusive_time_transition_compare(NULL, &b, NULL));
  tlib_pass_if_int_equal(
      "a NULL pointer is always considered equal to its counterpart", 0,
      nr_exclusive_time_transition_compare(&a, NULL, NULL));

  /*
   * Test : Time differs, type the same.
   */
  a.time = 10;
  b.time = 20;
  tlib_pass_if_int_equal("a.time < b.time; a.type == b.type", -1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  a.time = 30;
  tlib_pass_if_int_equal("a.time > b.time; a.type == b.type", 1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  /*
   * Test : Time the same, type the same.
   */
  a.time = 30;
  b.time = 30;
  tlib_pass_if_int_equal("a.time == b.time; a.type == b.type", 0,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  /*
   * Test : Time the same, type differs.
   */
  a.type = CHILD_STOP;
  tlib_pass_if_int_equal("a.time == b.time; a.type > b.type", 1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  a.type = CHILD_START;
  b.type = CHILD_STOP;
  tlib_pass_if_int_equal("a.time == b.time; a.type < b.type", -1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  /*
   * Test : Time differs, type differs.
   */
  a.time = 10;
  b.time = 20;
  tlib_pass_if_int_equal("a.time < b.time; a.type < b.type", -1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));

  a.time = 30;
  tlib_pass_if_int_equal("a.time > b.time; a.type < b.type", 1,
                         nr_exclusive_time_transition_compare(&a, &b, NULL));
}

tlib_parallel_info_t parallel_info
    = {.suggested_nthreads = -1, .state_size = 0};

void test_main(void* p NRUNUSED) {
  test_create_destroy();
  test_add_child();
  test_calculate();
  test_compare();
}
