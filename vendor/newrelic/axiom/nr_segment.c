#include "nr_axiom.h"

#include "nr_segment_private.h"
#include "nr_segment.h"
#include "nr_segment_traces.h"
#include "nr_txn.h"
#include "util_logging.h"
#include "util_memory.h"
#include "util_string_pool.h"
#include "util_time.h"
#include "util_logging.h"

nr_segment_t* nr_segment_start(nrtxn_t* txn,
                               nr_segment_t* parent,
                               const char* async_context) {
  nr_segment_t* new_segment;

  if (nrunlikely(NULL == txn)) {
    return NULL;
  }

  if (!txn->status.recording) {
    return NULL;
  }

  new_segment = nr_zalloc(sizeof(nr_segment_t));

  new_segment->color = NR_SEGMENT_WHITE;
  new_segment->type = NR_SEGMENT_CUSTOM;
  new_segment->txn = txn;

  /* A segment's time is expressed in terms of time relative to the transaction.
   * Determine the difference between the transaction's start time and now. */
  new_segment->start_time
      = nr_time_duration(nr_txn_start_time(txn), nr_get_time());

  new_segment->user_attributes = nro_new_hash();

  nr_segment_children_init(&new_segment->children);

  if (async_context) {
    new_segment->async_context
        = nr_string_add(txn->trace_strings, async_context);
  }

  /* If an explicit parent has been passed in, parent this newly
   * started segment with the explicit parent. Make the newly-started
   * segment a sibling of its parent's (possibly) already-existing children. */
  if (parent) {
    new_segment->parent = parent;
    nr_segment_children_add(&parent->children, new_segment);
  } /* Otherwise, the parent of this new segment is the current segment on the
       transaction */
  else {
    nr_segment_t* current_segment;
    current_segment = nr_txn_get_current_segment(txn);
    new_segment->parent = current_segment;

    if (NULL != current_segment) {
      nr_segment_children_add(&current_segment->children, new_segment);
    }
    nr_txn_set_current_segment(txn, new_segment);
  }
  return new_segment;
}

bool nr_segment_set_custom(nr_segment_t* segment) {
  if (NULL == segment) {
    return false;
  }

  if (NR_SEGMENT_CUSTOM == segment->type) {
    return true;
  }

  nr_segment_destroy_typed_attributes(segment->type,
                                      &segment->typed_attributes);
  segment->type = NR_SEGMENT_CUSTOM;

  return true;
}

bool nr_segment_set_datastore(nr_segment_t* segment,
                              const nr_segment_datastore_t* datastore) {
  if (nrunlikely((NULL == segment || NULL == datastore))) {
    return false;
  }

  nr_segment_destroy_typed_attributes(segment->type,
                                      &segment->typed_attributes);
  segment->type = NR_SEGMENT_DATASTORE;

  // clang-format off
  // Initialize the fields of the datastore attributes, one field per line.
  segment->typed_attributes.datastore = (nr_segment_datastore_t){
      .component = datastore->component ? nr_strdup(datastore->component) : NULL,
      .sql = datastore->sql ? nr_strdup(datastore->sql) : NULL,
      .sql_obfuscated = datastore->sql_obfuscated ? nr_strdup(datastore->sql_obfuscated) : NULL,
      .input_query_json = datastore->input_query_json ? nr_strdup(datastore->input_query_json) : NULL,
      .backtrace_json = datastore->backtrace_json ? nr_strdup(datastore->backtrace_json) : NULL,
      .explain_plan_json = datastore->explain_plan_json ? nr_strdup(datastore->explain_plan_json) : NULL,
  };

  segment->typed_attributes.datastore.instance = (nr_datastore_instance_t){
      .host = datastore->instance.host ? nr_strdup(datastore->instance.host) : NULL,
      .port_path_or_id = datastore->instance.port_path_or_id ? nr_strdup(datastore->instance.port_path_or_id) : NULL,
      .database_name = datastore->instance.database_name ? nr_strdup(datastore->instance.database_name): NULL,
  };
  // clang-format on

  return true;
}

bool nr_segment_set_external(nr_segment_t* segment,
                             const nr_segment_external_t* external) {
  if (nrunlikely((NULL == segment) || (NULL == external))) {
    return false;
  }

  nr_segment_destroy_typed_attributes(segment->type,
                                      &segment->typed_attributes);
  segment->type = NR_SEGMENT_EXTERNAL;

  // clang-format off
  // Initialize the fields of the external attributes, one field per line.
  segment->typed_attributes.external = (nr_segment_external_t){
      .transaction_guid = external->transaction_guid ? nr_strdup(external->transaction_guid) : NULL,
      .uri = external->uri ? nr_strdup(external->uri) : NULL,
      .library = external->library ? nr_strdup(external->library) : NULL,
      .procedure = external->procedure ? nr_strdup(external->procedure) : NULL,
  };
  // clang-format on

  return true;
}

bool nr_segment_add_child(nr_segment_t* parent, nr_segment_t* child) {
  if (nrunlikely((NULL == parent) || (NULL == child))) {
    return false;
  }

  nr_segment_set_parent(child, parent);

  return true;
}

static void nr_segment_metric_destroy_wrapper(void* sm,
                                              void* userdata NRUNUSED) {
  nr_segment_metric_destroy_fields((nr_segment_metric_t*)sm);
  nr_free(sm);
}

bool nr_segment_add_metric(nr_segment_t* segment,
                           const char* name,
                           bool scoped) {
  nr_segment_metric_t* sm;

  if (nrunlikely(NULL == segment || NULL == name)) {
    return false;
  }

  if (NULL == segment->metrics) {
    /* We'll use 4 as the default vector size here because that's the most
     * metrics we should see from an automatically instrumented segment: legacy
     * CAT will create scoped and unscoped rollup and ExternalTransaction
     * metrics. */
    segment->metrics
        = nr_vector_create(4, nr_segment_metric_destroy_wrapper, NULL);
  }

  sm = nr_malloc(sizeof(nr_segment_metric_t));
  sm->name = nr_strdup(name);
  sm->scoped = scoped;

  return nr_vector_push_back(segment->metrics, sm);
}

bool nr_segment_set_name(nr_segment_t* segment, const char* name) {
  if ((NULL == segment) || (NULL == name)) {
    return false;
  }

  segment->name = nr_string_add(segment->txn->trace_strings, name);

  return true;
}

bool nr_segment_set_parent(nr_segment_t* segment, nr_segment_t* parent) {
  nr_segment_t* ancestor = NULL;

  if (NULL == segment) {
    return false;
  }

  if (NULL != parent && segment->txn != parent->txn) {
    return false;
  }

  if (segment->parent == parent) {
    return true;
  }

  /*
   * Check if we are creating a cycle. If the to-be child segment is a child
   * of the to-be parent segment then we are creating a cycle. We should not
   * continue.
   */
  ancestor = parent;
  while (NULL != ancestor) {
    if (ancestor == segment) {
      nrl_warning(NRL_API,
                  "Unsuccessful call to newrelic_set_segment_parent(). Cannot "
                  "set parent because it would introduce a cycle into the "
                  "agent's call stack representation.");
      return false;
    }
    ancestor = ancestor->parent;
  }

  if (segment->parent) {
    nr_segment_children_remove(&segment->parent->children, segment);
  }

  nr_segment_children_add(&parent->children, segment);
  segment->parent = parent;

  return true;
}

bool nr_segment_set_timing(nr_segment_t* segment,
                           nrtime_t start,
                           nrtime_t duration) {
  if (NULL == segment) {
    return false;
  }

  segment->start_time = start;
  segment->stop_time = start + duration;

  return true;
}

bool nr_segment_end(nr_segment_t* segment) {
  nr_segment_t* current_segment = NULL;

  if (nrunlikely(NULL == segment) || (NULL == segment->txn)) {
    return false;
  }

  if (0 == segment->stop_time) {
    /* A segment's time is expressed in terms of time relative to the
     * transaction. Determine the difference between the transaction's start
     * time and now. */
    segment->stop_time
        = nr_time_duration(nr_txn_start_time(segment->txn), nr_get_time());
  }

  segment->txn->segment_count += 1;

  current_segment = nr_txn_get_current_segment(segment->txn);

  if (current_segment == segment) {
    nr_txn_retire_current_segment(segment->txn);
  }

  return true;
}

/*
 * Purpose : Given a segment color, return the other color.
 *
 * Params  : 1. The color to toggle.
 *
 * Returns : The toggled color.
 */
static nr_segment_color_t nr_segment_toggle_color(nr_segment_color_t color) {
  if (NR_SEGMENT_WHITE == color) {
    return NR_SEGMENT_GREY;
  } else {
    return NR_SEGMENT_WHITE;
  }
}

/*
 * Purpose : The callback registered by nr_segment_destroy_children_callback()
 *           to finish destroying the segment and (if necessary) its child
 *           structures.
 */
static void nr_segment_destroy_children_post_callback(nr_segment_t* segment,
                                                      void* userdata NRUNUSED) {
  /* Free the segment */
  nr_segment_destroy_fields(segment);
  nr_segment_children_destroy_fields(&segment->children);
  nr_free(segment);
}

/*
 * Purpose : The callback necessary to iterate over a
 * tree of segments and free them and all their children.
 */
static nr_segment_iter_return_t nr_segment_destroy_children_callback(
    nr_segment_t* segment,
    void* userdata NRUNUSED) {
  // If this segment has room for children, but no children,
  // then let's free the room for children.
  if (0 == nr_vector_size(&segment->children)) {
    nr_vector_deinit(&segment->children);
  }

  return ((nr_segment_iter_return_t){
      .post_callback = nr_segment_destroy_children_post_callback});
}

/*
 * Purpose : Iterate over the segments in a tree of segments.
 *
 * Params  : 1. A pointer to the root.
 *           2. The color of a segment not yet traversed.
 *           3. The color of a segment already traversed.
 *           4. The iterator function to be invoked for each segment
 *           5. Optional userdata for the iterator.
 *
 * Notes   : This iterator is hardened against infinite regress. Even
 *           when there are ill-formed cycles in the tree, the
 *           iteration will terminate because it colors the segments
 *           as it traverses them.
 */
static void nr_segment_iterate_helper(nr_segment_t* root,
                                      nr_segment_color_t reset_color,
                                      nr_segment_color_t set_color,
                                      nr_segment_iter_t callback,
                                      void* userdata) {
  if (NULL == root) {
    return;
  } else {
    // Color the segments as the tree is traversed to prevent infinite regress.
    if (reset_color == root->color) {
      nr_segment_iter_return_t cb_return;
      size_t i;
      size_t n_children = nr_vector_size(&root->children);

      root->color = set_color;

      // Invoke the pre-traversal callback.
      cb_return = (callback)(root, userdata);

      // Iterate the children.
      for (i = 0; i < n_children; i++) {
        nr_segment_iterate_helper(nr_vector_get(&root->children, i),
                                  reset_color, set_color, callback, userdata);
      }

      // If a post-traversal callback was registered, invoke it.
      if (cb_return.post_callback) {
        (cb_return.post_callback)(root, cb_return.userdata);
      }
    }
  }
}

void nr_segment_iterate(nr_segment_t* root,
                        nr_segment_iter_t callback,
                        void* userdata) {
  if (nrunlikely(NULL == callback)) {
    return;
  }

  if (nrunlikely(NULL == root)) {
    return;
  }
  /* What is the color of the root?  Assume the whole tree is that color.
   * The tree of segments is never partially traversed, so this assumption
   * is well-founded.
   *
   * That said, if there were a case in which the tree had been partially
   * traversed, and is traversed again, the worse case scenario would be that a
   * subset of the tree is not traversed. */
  nr_segment_iterate_helper(root, root->color,
                            nr_segment_toggle_color(root->color), callback,
                            userdata);
}

void nr_segment_destroy(nr_segment_t* root) {
  if (NULL == root) {
    return;
  }

  nr_segment_iterate(
      root, (nr_segment_iter_t)nr_segment_destroy_children_callback, NULL);
}

bool nr_segment_discard(nr_segment_t** segment_ptr) {
  nr_segment_t* segment;

  if (NULL == segment_ptr || NULL == *segment_ptr
      || NULL == (*segment_ptr)->txn) {
    return false;
  }

  segment = *segment_ptr;

  /* Don't discard root nodes. */
  if (NULL == segment->parent) {
    return false;
  }

  /* Reparent all children. */
  while (nr_vector_size(&segment->children) > 0) {
    bool rv = nr_segment_set_parent(nr_vector_get(&segment->children, 0),
                                    segment->parent);

    if (!rv) {
      return false;
    }
  }

  /* Unhook the segment from its parent. */
  nr_segment_children_remove(&segment->parent->children, segment);

  segment->txn->segment_count -= 1;

  /* Free memory. */
  nr_segment_destroy(segment);
  (*segment_ptr) = NULL;

  return true;
}

static int nr_segment_duration_comparator(const nr_segment_t* a,
                                          const nr_segment_t* b) {
  nrtime_t duration_a = a->stop_time - a->start_time;
  nrtime_t duration_b = b->stop_time - b->start_time;

  if (duration_a < duration_b) {
    return -1;
  } else if (duration_a > duration_b) {
    return 1;
  }
  return 0;
}

int nr_segment_wrapped_duration_comparator(const void* a,
                                           const void* b,
                                           void* userdata NRUNUSED) {
  return nr_segment_duration_comparator((const nr_segment_t*)a,
                                        (const nr_segment_t*)b);
}

nr_minmax_heap_t* nr_segment_heap_create(ssize_t bound,
                                         nr_minmax_heap_cmp_t comparator) {
  return nr_minmax_heap_create(bound, comparator, NULL, NULL, NULL);
}

static void nr_segment_stoh_post_iterator_callback(
    nr_segment_t* segment,
    nr_segment_tree_to_heap_metadata_t* metadata) {
  nrtime_t exclusive_time;
  size_t i;
  size_t metric_count;

  if (nrunlikely(NULL == segment || NULL == metadata)) {
    return;
  }

  // Calculate the exclusive time.
  exclusive_time = nr_exclusive_time_calculate(segment->exclusive_time);

  // Update the transaction total time.
  metadata->total_time += exclusive_time;

  // Merge any segment metrics with the transaction metric tables.
  metric_count = nr_vector_size(segment->metrics);
  for (i = 0; i < metric_count; i++) {
    nr_segment_metric_t* sm
        = (nr_segment_metric_t*)nr_vector_get(segment->metrics, i);

    nrm_add_ex(sm->scoped ? segment->txn->scoped_metrics
                          : segment->txn->unscoped_metrics,
               sm->name,
               nr_time_duration(segment->start_time, segment->stop_time),
               exclusive_time);
  }
}

/*
 * Purpose : Place an nr_segment_t pointer into a nr_minmax_heap,
 *             or "segments to heap".
 *
 * Params  : 1. The segment pointer to place into the heap.
 *           2. A void* pointer to be recast as the pointer to the heap.
 *
 * Note    : This is the callback function supplied to nr_segment_iterate(),
 *           used for iterating over a tree of segments and placing each
 *           segment into the heap.
 */
static nr_segment_iter_return_t nr_segment_stoh_iterator_callback(
    nr_segment_t* segment,
    void* userdata) {
  nr_minmax_heap_t* trace_heap = NULL;
  nr_minmax_heap_t* span_heap = NULL;
  nr_segment_tree_to_heap_metadata_t* metadata
      = (nr_segment_tree_to_heap_metadata_t*)userdata;

  if (nrunlikely(NULL == segment) || nrunlikely(NULL == userdata)) {
    return NR_SEGMENT_NO_POST_ITERATION_CALLBACK;
  }

  /* Set up the exclusive time so that children can adjust it as necessary. */
  nr_exclusive_time_destroy(&segment->exclusive_time);
  segment->exclusive_time
      = nr_exclusive_time_create(segment->start_time, segment->stop_time);

  /* Adjust the parent's exclusive time. */
  if (segment->parent
      && segment->parent->async_context == segment->async_context) {
    nr_exclusive_time_add_child(segment->parent->exclusive_time,
                                segment->start_time, segment->stop_time);
  }

  trace_heap = metadata->trace_heap;
  span_heap = metadata->span_heap;

  if (NULL != trace_heap) {
    nr_minmax_heap_insert(trace_heap, segment);
  }

  if (NULL != span_heap) {
    nr_minmax_heap_insert(span_heap, segment);
  }

  // clang-format off
  return ((nr_segment_iter_return_t){
    .post_callback
      = (nr_segment_post_iter_t)nr_segment_stoh_post_iterator_callback,
    .userdata = metadata,
  });
  // clang-format on
}

void nr_segment_tree_to_heap(nr_segment_t* root,
                             nr_segment_tree_to_heap_metadata_t* metadata) {
  if (NULL == root || NULL == metadata) {
    return;
  }
  /* Convert the tree to two minmax heap.  The bound, or
   * size, of the heaps, and the comparison functions installed
   * by the nr_segment_heap_create() calls will assure that the
   * segments in the heaps are of highest priority. */
  nr_segment_iterate(root, (nr_segment_iter_t)nr_segment_stoh_iterator_callback,
                     metadata);
}

/*
 * Purpose : Place an nr_segment_t pointer in a heap into a nr_set_t,
 *             or "heap to set".
 *
 * Params  : 1. The segment pointer in the heap.
 *           2. A void* pointer to be recast as the pointer to the set.
 *
 * Note    : This is the callback function supplied to nr_minmax_heap_iterate
 *           used for iterating over a heap of segments and placing each
 *           segment into a set.
 */
static bool nr_segment_htos_iterator_callback(void* value, void* userdata) {
  if (nrlikely(value && userdata)) {
    nr_set_t* set = (nr_set_t*)userdata;
    nr_set_insert(set, value);
  }

  return true;
}

void nr_segment_heap_to_set(nr_minmax_heap_t* heap, nr_set_t* set) {
  if (NULL == heap || NULL == set) {
    return;
  }

  /* Convert the heap to a set */
  nr_minmax_heap_iterate(
      heap, (nr_minmax_heap_iter_t)nr_segment_htos_iterator_callback,
      (void*)set);

  return;
}
