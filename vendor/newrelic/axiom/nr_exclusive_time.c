#include "nr_exclusive_time.h"
#include "nr_exclusive_time_private.h"

#include "nr_axiom.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_time.h"
#include "util_vector.h"

static void nr_exclusive_time_transition_destroy(
    nr_exclusive_time_transition_t* transition,
    void* userdata NRUNUSED) {
  nr_free(transition);
}

nr_exclusive_time_t* nr_exclusive_time_create(nrtime_t start_time,
                                              nrtime_t stop_time) {
  nr_exclusive_time_t* et;

  if (start_time > stop_time) {
    return NULL;
  }

  et = nr_malloc(sizeof(nr_exclusive_time_t));
  et->start_time = start_time;
  et->stop_time = stop_time;

  /*
   * 32 is the closest power of two to twice the average number of child
   * segments on a PHP agent segment (which is about ten). Since this is only
   * used while assembling the segment tree, we're less concerned with memory
   * usage at this point and more concerned with execution time: we'll try to
   * avoid a reallocation in the normal case by allocating a bit more meory up
   * front.
   */
  nr_vector_init(&et->transitions, 32,
                 (nr_vector_dtor_t)nr_exclusive_time_transition_destroy, NULL);

  return et;
}

bool nr_exclusive_time_destroy(nr_exclusive_time_t** et_ptr) {
  if (NULL == et_ptr || NULL == *et_ptr) {
    return false;
  }

  nr_vector_deinit(&((*et_ptr)->transitions));
  nr_realfree((void**)et_ptr);

  return true;
}

bool nr_exclusive_time_add_child(nr_exclusive_time_t* parent_et,
                                 nrtime_t start_time,
                                 nrtime_t stop_time) {
  nr_exclusive_time_transition_t* start;
  nr_exclusive_time_transition_t* stop;

  if (NULL == parent_et) {
    return false;
  }

  if (start_time > stop_time) {
    nrl_verbosedebug(NRL_TXN,
                     "cannot have start time " NR_TIME_FMT
                     " > stop time " NR_TIME_FMT,
                     start_time, stop_time);
    return false;
  }

  /*
   * Short circuit: child segments completely outside their parents time window
   * cannot affect exclusive time, so don't bother recording it.
   */
  if (stop_time < parent_et->start_time || start_time > parent_et->stop_time) {
    return true;
  }

  /*
   * Basic theory of operation: we need to add a transition for both the start
   * and stop of this segment to the transitions vector.
   */
  start = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  start->time = start_time;
  start->type = CHILD_START;
  nr_vector_push_back(&parent_et->transitions, start);

  stop = nr_malloc(sizeof(nr_exclusive_time_transition_t));
  stop->time = stop_time;
  stop->type = CHILD_STOP;
  nr_vector_push_back(&parent_et->transitions, stop);

  return true;
}

int nr_exclusive_time_transition_compare(
    const nr_exclusive_time_transition_t* a,
    const nr_exclusive_time_transition_t* b,
    void* userdata NRUNUSED) {
  if (nrunlikely(NULL == a || NULL == b)) {
    /*
     * There really shouldn't be a scenario in which a NULL pointer finds its
     * way into this vector.
     */
    nrl_error(NRL_TXN,
              "unexpected NULL pointer when comparing transitions: a=%p; b=%p",
              a, b);
    return 0;
  }

  if (a->time < b->time) {
    return -1;
  } else if (a->time > b->time) {
    return 1;
  }

  /*
   * Starts go before stops, if the times are equal. (There's no functional
   * difference in doing so, but it saves a tiny bit of work in
   * nr_exclusive_time_calculate() updating exclusive_time and last_start if we
   * stop the active counter dropping to 0.)
   */
  if (a->type != b->type) {
    if (CHILD_START == a->type) {
      return -1;
    } else if (CHILD_START == b->type) {
      return 1;
    }
  }

  // Eh, whatever. Some things _are_ created equal.
  return 0;
}

nrtime_t nr_exclusive_time_calculate(nr_exclusive_time_t* et) {
  unsigned int active_children = 0;
  nrtime_t exclusive_time;
  size_t i;
  nrtime_t last_start;
  size_t num_transitions;

  if (NULL == et) {
    return 0;
  }

  /*
   * Essentially, what we want to do in this function is walk the list of
   * transitions in time order. So, firstly, let's put it in time order.
   */
  if (!nr_vector_sort(&et->transitions,
                      (nr_vector_cmp_t)nr_exclusive_time_transition_compare,
                      NULL)) {
    nrl_warning(NRL_TXN, "error sorting transitions");
    return 0;
  }

  /*
   * It's generally easier to reason about exclusive time if you think of it as
   * a subtractive process: all time that cannot be attributed to a direct
   * child is exclusive time, since it represents time the segment in question
   * was doing stuff. So we'll start by setting the exclusive time to be the
   * full duration of the segment.
   */
  exclusive_time = nr_time_duration(et->start_time, et->stop_time);

  num_transitions = nr_vector_size(&et->transitions);
  for (i = 0; i < num_transitions; i++) {
    nr_exclusive_time_transition_t* trans = nr_vector_get(&et->transitions, i);

    if (NULL == trans) {
      nrl_warning(NRL_TXN, "unexpected NULL transition at index %zu; ignoring",
                  i);
      continue;
    }

    if (CHILD_START == trans->type) {
      /*
       * OK, so we have a start transition. If there are no active children,
       * then that means that the exclusive time for the segment ends at this
       * point, so we'll track this time as the last start.
       *
       * If the child segment is starting _before_ the parent segment (which is
       * possible in an async world), then we'll just clamp the time to the
       * segment start time for now and see what else we get.
       */
      if (0 == active_children) {
        if (trans->time > et->start_time) {
          last_start = trans->time;
        } else {
          last_start = et->start_time;
        }
      }
      active_children += 1;
    } else if (CHILD_STOP == trans->type) {
      /*
       * Here we have a stop transition. If this is the last active child, then
       * this is the end of the period of non-exclusive time, and we should
       * adjust the segment's exclusive time accordingly.
       */
      if (0 == active_children) {
        nrl_warning(NRL_TXN,
                    "child stopped, but no children were thought to be active");
        continue;
      } else if (1 == active_children) {
        nrtime_t duration;

        /*
         * As with start transitions, nothing can happen to the segment's
         * exclusive time after the stop time, so we'll clamp the duration to
         * the stop time if required.
         */
        if (et->stop_time < trans->time) {
          duration = nr_time_duration(last_start, et->stop_time);
        } else {
          duration = nr_time_duration(last_start, trans->time);
        }

        if (duration > exclusive_time) {
          /*
           * Hitting this arm is probably a logic bug.
           */
          nrl_verbosedebug(NRL_TXN,
                           "attempted to subtract " NR_TIME_FMT
                           " us from exclusive time of " NR_TIME_FMT
                           " us; this should be impossible",
                           duration, exclusive_time);
          exclusive_time = 0;
          break;
        } else {
          exclusive_time -= duration;
        }

        /*
         * If we're past the end of the parent, we can just bail early; nothing
         * else can affect the exclusive time from here on, since we know the
         * vector is sorted.
         */
        if (trans->time > et->stop_time) {
          break;
        }
      }

      active_children -= 1;
    } else {
      nrl_error(NRL_TXN, "unknown transition type %d", (int)trans->type);
    }
  }

  return exclusive_time;
}
