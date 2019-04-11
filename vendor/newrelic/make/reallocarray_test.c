#include <stdlib.h>

int main(void) {
  free(reallocarray(NULL, 8, 8));
  return 0;
}
