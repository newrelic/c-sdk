#include "util_strings.h"
#include "libnewrelic.h"

const char* newrelic_version(void) {
  return NR_STR2(NEWRELIC_VERSION);
}
