#include "libnewrelic.h"

void do_transaction_and_error(newrelic_app_t* app,
                              const char* transaction_name,
                              const char* error_name);
newrelic_app_t* newrelic_get_app(void);
