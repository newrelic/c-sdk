#include "nr_axiom.h"
#include "util_stack.h"

bool nr_stack_init(nr_stack_t* s, size_t capacity) {
  if (0 == capacity) {
    return false;
  }

  return nr_vector_init(s, capacity, NULL, NULL);
}

void* nr_stack_get_top(nr_stack_t* s) {
  size_t size = nr_vector_size(s);

  if (nrunlikely(0 == size)) {
    return NULL;
  }

  return nr_vector_get(s, size - 1);
}

void nr_stack_push(nr_stack_t* s, void* new_element) {
  nr_vector_push_back(s, new_element);
}

void* nr_stack_pop(nr_stack_t* s) {
  void* element = NULL;

  nr_vector_pop_back(s, &element);
  return element;
}

void nr_stack_destroy_fields(nr_stack_t* s) {
  nr_vector_deinit(s);
}
