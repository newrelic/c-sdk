#include <algorithm>
#include <chrono>
#include <list>
#include <random>
#include <thread>

#include <boost/format.hpp>

#include "random.h"
#include "threads.h"

static void segmentThread(Transaction& txn,
                          std::string id,
                          unsigned int segments,
                          unsigned int maxTime) {
  auto generator(defaultGenerator());
  std::uniform_int_distribution<unsigned int> distribution(1, maxTime);

  for (unsigned int i = 0; i < segments; i++) {
    auto segment(randomSegment(txn));

    std::this_thread::sleep_for(
        std::chrono::milliseconds(distribution(generator)));

    segment->end();
  }
}

void transactionThread(Application& app,
                       std::string id,
                       unsigned int threads,
                       unsigned int segments,
                       unsigned int maxTime) {
  Transaction txn(app, id, false);
  std::list<std::thread> segmentThreads;

  // Start all the threads.
  for (unsigned int i = 0; i < threads; i++) {
    std::string threadID(
        boost::str(boost::format("%s segment thread %u") % id % i));

    segmentThreads.push_back(
        std::thread(segmentThread, std::ref(txn), threadID, segments, maxTime));
  }

  // Wait for them to complete.
  std::for_each(segmentThreads.begin(), segmentThreads.end(),
                [](std::thread& thread) { thread.join(); });
}
