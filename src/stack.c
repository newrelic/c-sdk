#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif

#include "libnewrelic.h"
#include "stack.h"

#include "util_memory.h"
#include "util_object.h"
#include "util_strings.h"

#define NUM_BACKTRACE_FRAMES 100

char* newrelic_get_stack_trace_as_json(void) {
#ifdef HAVE_BACKTRACE
  size_t size_of_backtrace_array;
  void* backtrace_pointers[NUM_BACKTRACE_FRAMES];
  char* stacktrace_json;
  nrobj_t* arr_backtrace;
  char** backtrace_lines;

  // grab the backtrace lines
  size_of_backtrace_array = backtrace(backtrace_pointers, NUM_BACKTRACE_FRAMES);
  backtrace_lines
      = backtrace_symbols(backtrace_pointers, size_of_backtrace_array);

  // for each line in the backtrace, add to our nro_array
  arr_backtrace = nro_new_array();
  for (size_t i = 0; i < size_of_backtrace_array; i++) {
    nro_set_array_string(arr_backtrace, 0, backtrace_lines[i]);
  }

  // serialize the nro_array as json
  stacktrace_json = nro_to_json(arr_backtrace);

  // free up what we don't need
  nr_free(backtrace_lines);
  nro_delete(arr_backtrace);
  return stacktrace_json;
#else
  return nr_strdup("[\"No backtrace on this platform.\"]");
#endif
}
