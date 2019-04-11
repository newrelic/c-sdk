# New Relic C SDK

A generic library to communicate with New Relic from any language with a C FFI
mechanism.

## Building the agent

### Requirements

The C SDK works on 64-bit Linux operating systems with:

* gcc 4.8 or higher
* glibc 2.17 or higher
* Kernel version 2.6.26 or higher
* libpcre 8.20 or higher
* libpthread

Running unit tests requires cmake 2.8 or higher.

Compiling the New Relic daemon requires Go 1.4 or higher.

### Building

Building the agent and daemon should be as simple as:

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

## Using the agent

API usage information can be found in [the guide](GUIDE.md).

### Headers

Note that only `include/libnewrelic.h` contains the stable, public API. Other
header files are internal to the agent, and their stability is not guaranteed.

### API reference

You can use [doxygen](http://www.doxygen.nl/) to generate API reference
documentation:

```sh
doxygen
```

This will create HTML output in the `html` directory.

## Running tests

### Unit tests

To compile and run the unit tests:

```sh
make run_tests
```

Or, just to compile them:

```sh
make tests
```

### Ad hoc tests

A number of ad hoc tests have been provided. More information can be found in
[the ad hoc test readme](tests/adhoc/README.md).
