#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <string>
#include <thread>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "threads.h"

namespace po = boost::program_options;

static void requireMin(const char* name, unsigned int min, unsigned int value) {
  if (value < min) {
    throw po::validation_error(po::validation_error::invalid_option_value,
                               name);
  }
}

static po::variables_map parseOptions(int argc, char* argv[]) {
  po::options_description desc("Allowed options");
  po::variables_map vm;

  desc.add_options()("help", "show this message")(
      "licence,l", po::value<std::string>(), "set the New Relic licence key")(
      "license", po::value<std::string>(), "set the New Relic license key")(
      "appname,a", po::value<std::string>()->default_value("C Threads"),
      "set the New Relic application name")(
      "host,h",
      po::value<std::string>()->default_value("collector.newrelic.com"),
      "set the New Relic host")(
      "transactions,x",
      po::value<unsigned int>()->default_value(10)->notifier(
          [](unsigned int value) { requireMin("transaction", 1, value); }),
      "number of transactions to create")(
      "threads,t",
      po::value<unsigned int>()->default_value(100)->notifier(
          [](unsigned int value) { requireMin("threads", 1, value); }),
      "number of threads each transaction should spawn")(
      "segments,s",
      po::value<unsigned int>()->default_value(20)->notifier(
          [](unsigned int value) { requireMin("segments", 1, value); }),
      "number of segments each thread should create")(
      "max-time,m",
      po::value<unsigned int>()->default_value(100)->notifier(
          [](unsigned int value) { requireMin("max-time", 1, value); }),
      "maximum time a segment can be, in milliseconds");

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (po::validation_error& e) {
    std::cerr << "Error parsing command line options: " << e.what()
              << std::endl;
    std::exit(1);
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    std::exit(0);
  }

  if (vm.count("licence") == 0 && vm.count("license") == 0) {
    std::cerr << "Error parsing command line options: a licence key must be "
                 "provided"
              << std::endl;
    std::exit(1);
  }

  return vm;
}

int main(int argc, char* argv[]) {
  po::variables_map vm(parseOptions(argc, argv));

  newrelic_configure_log("stdout", NEWRELIC_LOG_DEBUG);

  // Set up the agent configuration.
  Config config(vm["appname"].as<std::string>(),
                vm.count("licence") ? vm["licence"].as<std::string>()
                                    : vm["license"].as<std::string>());

  std::strncpy(config.config->redirect_collector,
               vm["host"].as<std::string>().c_str(), 255);
  config.config->redirect_collector[254] = '\0';

  // Wait up to five seconds for the application to connect.
  Application app(config, 5000);

  // Spawn threads.
  std::list<std::thread> txnThreads;

  auto transactions = vm["transactions"].as<unsigned int>();
  for (unsigned int i = 0; i < transactions; i++) {
    std::string id(boost::str(boost::format("%u") % i));

    txnThreads.push_back(std::thread(
        transactionThread, std::ref(app), id, vm["threads"].as<unsigned int>(),
        vm["segments"].as<unsigned int>(), vm["max-time"].as<unsigned int>()));
  }

  // Wait for them to complete.
  std::for_each(txnThreads.begin(), txnThreads.end(),
                [](std::thread& thread) { thread.join(); });

  return 0;
}
