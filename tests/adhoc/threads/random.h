#ifndef THREADS_RANDOM_HDR
#define THREADS_RANDOM_HDR

#include <chrono>
#include <random>

inline std::default_random_engine defaultGenerator() {
  return std::default_random_engine(
      std::chrono::system_clock::now().time_since_epoch().count());
}

#endif /* THREADS_RANDOM_HDR */
