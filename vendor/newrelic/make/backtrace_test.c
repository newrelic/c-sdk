#include <execinfo.h>

int main(void) {
  void* arr[100];

  backtrace(arr, sizeof(arr) / sizeof(void*));
  backtrace_symbols_fd(arr, sizeof(arr) / sizeof(void*), 1);

  return 0;
}
