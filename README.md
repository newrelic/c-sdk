# New Relic C SDK

This is the New Relic C SDK! If your application does not use other New Relic 
APM agent languages, you can use the C SDK to take advantage of New Relic's
monitoring capabilities and features to instrument a wide range of applications.

[![Build Status](https://travis-ci.org/newrelic/c-sdk.svg?branch=master)](https://travis-ci.org/newrelic/c-sdk)

## Need Help?

Recommendations for learning more:

* See the Docs site's 
[landing page](https://docs.newrelic.com/docs/c-sdk-table-contents)
for C SDK documentation.

* See New Relic's Documentation available at 
[C SDK documentation on GitHub](https://newrelic.github.io/c-sdk/index.html).

* Browse 
[New Relic's Explorers Hub](https://discuss.newrelic.com/c/build-on-new-relic/Open-Source-Agents-SDKs)
for community discussions about the C SDK.

* Use your preferred search engine to find other New Relic resources.

## Contributing

We'd love to get your contributions to improve the C SDK! Keep in mind when you 
submit your pull request, you'll need to sign the CLA via the click-through 
using CLA-Assistant. If you'd like to execute our corporate CLA, or if you 
have any questions, please drop us an email at open-source@newrelic.com.

## Requirements

The C SDK works on 64-bit Linux operating systems with:

* gcc 4.8 or higher
* glibc 2.17 or higher
* Kernel version 2.6.26 or higher
* libpcre 8.20 or higher
* libpthread

Running unit tests requires cmake 2.8 or higher.

Compiling the New Relic daemon requires Go 1.4 or higher.

## Building the C SDK

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

Then you can invoke your instrumented application.  Your application,
which makes C SDK API calls, reports data to the daemon over a socket;
in turn, the daemon reports the data to New Relic.


## Tests

To compile and run the unit tests:

```sh
make run_tests
```

Or, just to compile them:

```sh
make tests
```

## Licensing

The New Relic C SDK 
[is licensed under the Apache 2.0 License](https://github.com/newrelic/c-sdk/tree/master/LICENSE)

The C SDK also uses source code from third party libraries. Full details on
which libraries are used and the terms under which they are licensed can be
found in the 
[third party notices document](https://github.com/newrelic/c-sdk/tree/master/THIRD_PARTY_NOTICES.md).