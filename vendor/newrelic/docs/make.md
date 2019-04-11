# PHP agent `make` reference

Our top level Makefile is the main entry point to build the agent, daemon, and
their various dependencies. It requires GNU make, and almost certainly won't
work on other make implementations.

All targets should support parallel compilation using
[`-j`](https://www.gnu.org/software/make/manual/html_node/Parallel.html). If
they don't, please report a bug!

## Targets

### Build

#### `all`

By default, the agent, daemon, and integration runner will all be built. The
agent can be found in `agent/.libs/newrelic.so`, and the daemon and associated
Go binaries can be found in `bin`.

#### `agent`

Just builds the agent.

#### `agent-install`

Builds the agent and installs it to the active PHP installation.

#### `daemon`

Just builds the daemon, integration runner, and friends. (In effect, this
builds whatever Go packages we have in `src`.)

### Testing

#### `check` or `run_tests`

Builds the agent and axiom, then builds their unit test suites and runs them.

#### `valgrind`

As above, except the tests get run under valgrind and fail if any memory leaks
occur.

#### `agent-run-tests`

Just builds and runs the agent unit tests.

#### `agent-valgrind`

Builds the agent unit tests, then runs them under valgrind, failing if any
memory leaks occur. (Or if any tests fail.)

#### `axiom-run-tests`

Just builds and runs the axiom unit tests. The tests that are to be run can be
controlled by the target-specific `TESTS` and `SKIP_TESTS` variables, like so:

```sh
make axiom-run-tests TESTS=test_txn
make axiom-run-tests SKIP_TESTS=test_rpm
```

#### `axiom-valgrind`

Builds the axiom unit tests, then runs them under valgrind, failing if any
memory leaks occur. (Or if any tests fail.)

#### `integration`

Runs the agent integration tests against every supported version of PHP.
Unlike almost every other target, this requires NRLAMP.

#### `daemon_integration`

In spite of its name, this builds and runs the daemon unit tests.

#### `daemon_bench`

Runs the daemon performance tests.

#### `coverage`
Run integration and unit tests and create an html file that contains a test 
code coverage report. This file will be named `coverage-report.html`. More 
information [here](development_guide.md#code-coverage)


### Cleaning

#### `clean`

Cleans all the things! This should return your working directory to a pristine,
slightly minty state. Both this target and `agent-clean` require a working
`phpize` to be in your PATH.

#### `agent-clean`

Just cleans the agent. Useful if you want to build against a different PHP
version but don't want to rebuild axiom or the daemon.

#### `axiom-clean`

Just cleans axiom.

#### `daemon-clean`

Just cleans the daemon.

### Debugging and code coverage

#### `gcov`

Will run [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) against any `.gcno` files found in the working directory. To actually generate those files, you should run the test suite you're interested in with `ENABLE_COVERAGE=1`; eg:

```sh
make axiom-run-tests ENABLE_COVERAGE=1
make gcov
```

To create an html coverage report based on a run of the unit tests and integration tests run:
```sh
make coverage
```

## Variables

### PHP

You can set the `phpize` and `php-config` binaries that will be used via the
`PHPIZE` and `PHP_CONFIG` variables, respectively. For example, to use a PHP
installation in `/opt/php/5.5`:

```sh
make PHPIZE=/opt/php/5.5/bin/phpize PHP_CONFIG=/opt/php/5.5/bin/php-config
```

### Optimisation

#### `OPTIMIZE`

When set to `1`, this enables optimisation in the C compiler. This variable is
used for release builds, so if you want something that matches a release build,
you should set this.

#### `ENABLE_LTO`

When set to `1`, this enables link time optimisation.

### Testing

#### `ENABLE_COVERAGE`

When set to `1`, this enables `gcov` support when building the agent.

#### `SANITIZE`

When set, the value will be passed to the C compiler via the `-fsanitize` flag.
This can be used to enable the address or thread sanitisers provided in recent
GCC or Clang versions.

Useful values include `address` and `thread` to run the address and thread
sanitisers, respectively.

### General

An incomplete list of useful variables follows:

* `AR`: the archiver that will be used
* `CFLAGS`: flags given to the C compiler
* `CC`: the C compiler that will be used
* `CPPFLAGS`: flags given to the C compiler when building a `.c` file
* `GO`: the Go compiler that will be used
* `LDFLAGS`: flags given to the linker when linking a shared library or binary

### Technically unrelated things

The Makefile used to document a few recipes for running the agent in specific
environments. These may or may not still apply, and are reproduced below for
posterity.

#### Thread sanitiser

To compile and run the axiom tests using the thread sanitizer from gcc 4.8.3
(from a modern nrcamp), do:

```sh
make check SANITIZE=thread
```

See https://code.google.com/p/thread-sanitizer/wiki/CppManual for a discussion
on how to run on Ubuntu.

1. You need to turn on ASLR do: `echo 2 > /proc/sys/kernel/randomize_va_space`
2. Under gdb do: `set disable-randomization off`

#### glibc malloc checking

First, read the man page for [mallopt](http://man7.org/linux/man-pages/man3/mallopt.3.html).

Then:

```sh
make clean axiom-tests
MALLOC_CHECK_=7 make axiom-run-tests
```

#### jemalloc

Once installed, you may be able to run the agent with
[jemalloc](https://github.com/jemalloc/jemalloc) with this recipe:

```sh
make clean axiom-tests
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.1 MALLOC_CONF="abort:true" make axiom-run-tests
```

#### tcmalloc

Similarly, [tcmalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html)
is available, should you wish to run the agent using its heap checker:

```sh
make clean axiom-tests
LD_PRELOAD=/usr/lib/libtcmalloc.so HEAPCHECK=normal make axiom-run-tests
```
