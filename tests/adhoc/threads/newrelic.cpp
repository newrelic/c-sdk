#include <random>
#include <stdexcept>
#include <iostream>

#include "newrelic.h"
#include "random.h"

std::unique_ptr<Segment> randomSegment(Transaction& txn) {
  auto generator(defaultGenerator());
  std::uniform_int_distribution<int> distribution(0, 2);

  switch (distribution(generator)) {
    case 0:
      return std::make_unique<Segment>(CustomSegment(txn, "Random", "Custom"));

    case 1:
      return std::make_unique<Segment>(DatastoreSegment(txn));

    case 2:
      return std::make_unique<Segment>(ExternalSegment(txn));

    default:
      throw std::logic_error("unexpected segment type");
  }
}
