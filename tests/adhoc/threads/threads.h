#ifndef THREADS_HDR
#define THREADS_HDR

#include <string>

#include "newrelic.h"

extern void transactionThread(Application& app,
                              std::string id,
                              unsigned int threads,
                              unsigned int segments,
                              unsigned int maxTime);

#endif /* THREADS_HDR */
