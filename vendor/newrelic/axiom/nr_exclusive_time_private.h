#ifndef NR_EXCLUSIVE_TIME_PRIVATE_HDR
#define NR_EXCLUSIVE_TIME_PRIVATE_HDR

struct _nr_exclusive_time_t {
  nrtime_t start_time;
  nrtime_t stop_time;
  nr_vector_t transitions;
};

/*
 * A record of a state transition: either the start or end (stop) of a child
 * segment.
 *
 * We'd probably call these "events" were it not for the heavily overloaded use
 * of that noun already.
 */
typedef struct _nr_exclusive_time_transition_t {
  nrtime_t time;
  enum {
    CHILD_START,
    CHILD_STOP,
  } type;
} nr_exclusive_time_transition_t;

extern int nr_exclusive_time_transition_compare(
    const nr_exclusive_time_transition_t* a,
    const nr_exclusive_time_transition_t* b,
    void* userdata);

#endif /* NR_EXCLUSIVE_TIME_PRIVATE_HDR */
