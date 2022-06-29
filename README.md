[![Archived header](https://github.com/newrelic/opensource-website/raw/master/src/images/categories/Archived.png)](https://opensource.newrelic.com/oss-category/#archived)

# New Relic C SDK

This is the New Relic C SDK! If your application does not use other New Relic 
APM agent languages, you can use the C SDK to take advantage of New Relic's
monitoring capabilities and features to instrument a wide range of applications.

[![Build Status](https://travis-ci.org/newrelic/c-sdk.svg?branch=master)](https://travis-ci.org/newrelic/c-sdk)

## End of Life Notice ##
As of April 2022, we're discontinuing support for several capabilities, including the C SDK. Please explore the [option to use OpenTelemetry](https://github.com/newrelic/docs-website/blob/develop/docs/apm/agents/c-sdk/get-started/otel_cpp_example) as an alternative to send C++ telemetry data to New Relic. For more details about the EOL, see our [Explorers Hub post](https://discuss.newrelic.com/t/q1-bulk-eol-announcement-fy23/181744).

## Installation

### Requirements

The C SDK works on 64-bit Linux operating systems with:

* gcc 4.8 or higher
* glibc 2.17 or higher
* Kernel version 2.6.26 or higher
* libpcre 8.20 or higher
* libpthread

Running unit tests requires cmake 2.8 or higher.

Compiling the New Relic daemon requires Go 1.7 or higher.

## Building

If your system meets the requirements, building the C SDK and 
daemon should be as simple as:

```sh
make
```

This will create two files in this directory:

* `libnewrelic.a`: the static C SDK library, ready to link against.
* `newrelic-daemon`: the daemon binary, ready to run.

You can start the daemon in the foreground with:

```sh
./newrelic-daemon -f --logfile stdout --loglevel debug
```

Alternatively, you can use the [C SDK daemon image on
Dockerhub](https://hub.docker.com/r/newrelic/c-daemon) to run the daemon.

Then you can invoke your instrumented application.  Your application,
which makes C SDK API calls, reports data to the daemon over a socket;
in turn, the daemon reports the data to New Relic.

For more information on installation and configuration of the C SDK for
different environments, see [Install and configure](https://docs.newrelic.com/docs/agents/c-sdk/install-configure).

## Testing

To compile and run the unit tests:

```sh
make run_tests
```

Or, just to compile them:

```sh
make tests
```

## Privacy
At New Relic we take your privacy and the security of your information seriously, and are committed to protecting your information. We must emphasize the importance of not sharing personal data in public forums, and ask all users to scrub logs and diagnostic information for sensitive information, whether personal, proprietary, or otherwise.

We define “Personal Data” as any information relating to an identified or identifiable individual, including, for example, your name, phone number, post code or zip code, Device ID, IP address and email address.

For more information, review [New Relic’s General Data Privacy Notice](https://newrelic.com/termsandconditions/privacy).

## Contribute

We encourage your experiments to improve the C SDK in your personal forks.

**A note about vulnerabilities**

As noted in our [security policy](https://github.com/newrelic/c-sdk/security/policy), New Relic is committed to the privacy and security of our customers and their data. We believe that providing coordinated disclosure by security researchers and engaging with the security community are important means to achieve our security goals.

If you believe you have found a security vulnerability in this project or any of New Relic's products or websites, we welcome and greatly appreciate you reporting it to New Relic through [HackerOne](https://hackerone.com/newrelic).

If you would like to contribute to this project, review [these guidelines](./CONTRIBUTING.md).

To [all contributors](https://github.com/newrelic/c-sdk/graphs/contributors), we thank you!  Without your contribution, this project would not be what it is today.  

## License
The New Relic C SDK is licensed under the [Apache 2.0](http://apache.org/licenses/LICENSE-2.0.txt) License.

The C SDK also uses source code from third party libraries. Full details on
which libraries are used and the terms under which they are licensed can be
found in the 
[third party notices document](https://github.com/newrelic/c-sdk/tree/master/THIRD_PARTY_NOTICES.md).
