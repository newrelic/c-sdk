Integration Runner: Theory of Operation
--------------------------------------------------
The integration runner is a binary application that runs PHP integration tests.  It's written in Go, and reuses components of the daemon application.

When diagnosing integration test failures, it's important to understand how the integration runner does what it does.  The best way to understand the integration runner is to compare its operation to the normal operation of the PHP agent.

PHP Agent: Normal Operation
--------------------------------------------------
In a standard customer setup, there are three layers to the PHP agent.

- The `newrelic.so` PHP extension
- The `newrelic-daemon` background application (i.e. daemon)
- The New Relic Collector

Our customers install the `newrelic.so` extension on their servers. The `newrelic.so` extension performs instrumentation on running PHP processes.  The `newrelic.so` extension sends this instrumentation to the `newrelic-daemon` background application via unix sockets or TCP/IP.  Our customers also install the `newrelic-daemon` on their servers.

The `newrelic-daemon` collects and holds this instrumentation data.  On a regular schedule, (once per minute for most customers) the daemon will send the information it has collected to New Relic's collector server.  This is known as a harvest cycle.  The daemon also has a set of rules, logic, and limits for how long to hold onto this data, and how it should cull the data when it needs to drop metrics, events, or traces.  These limits are in place to ensure the agent does not cause resource issues on our customer's servers.

The New Relic collector **is not** installed on our customer's servers.  It lives "in the cloud" (i.e. a New Relic hosted data center).

Integration Runner
--------------------------------------------------
The integration runner sets up an environment similar to the standard customer setup, with a few important differences.

When running integration tests, we point the integration runner at a specific version of the `newrelic.so` file we want to test, and a specific version of PHP.

However, we **do not** point the integration runner at a `newrelic-daemon`.  Instead, the integration runner *acts as* the daemon.  Since the integration runner is written in Go, the integration runner can import daemon libraries and behave in a way that is similar to the daemon we supply to customers.

In addition to pointing the integration runner to a `newrelic.so` and a `php` binary, we also point the integration runner to a folder containing `test_*.php` files.  These files are integration runner test cases.

When the integration runner starts, it still performs a collector handshake to validate the application and grab collector-provided configuration variables.  The integration runner will look at each `test_*.php` test case and use the PHP binary to run the test case's code.  The integration runner will still accept requests from the `newrelic.so` PHP extension. However, the integration runner **does not** perform a harvest cycle, and will not sent information to the collector.

Instead, after running through each test, the integration runner will look at the metrics, events, and traces sent by the test, compare them to the *expected* values for a test run, and report `ok`, `not ok`, `skip`, or `xfail` (expected fail) for each test.  See the Test Format section below for a full list of *EXPECTED_* directives.

If you're a fan of listed steps, here's another way of saying the above.

1. PHP test case code runs
2. The `newrelic.so` extension collects instrumentation as the PHP code runs
3. The `newrelic.so` extension sends instrumentation *to the integration-runner-acting-as-daemon*
4. The *integration-runner-acting-as-daemon* holds on to the metrics

### Integration Runner CLI Flags

The integration runner accepts a number of optional command line flags.  For example, the command below uses the `agent` flag to test a specific `newrelic.so` file.

    ./bin/integration_runner -agent agent/.libs/newrelic.so \
      -php $(which php) /path/to/multiverse/laravel/4.0

Here's a brief description of each flag -- if you're curious about specific implementation details these flags are parsed in `./src/integration_runner/main.go`.

`-agent` : The agent flag allows users specify which PHP extension the integration runner will test

`-cgi` : The cgi flag allows users to specify which `php-cgi` binary the integration runner will use when running "web" tests.

`-collector` : The collector flag allows users to specify which New Relic collector service/URL to communicate/handshake with.

`-loglevel` : The loglevel flag allows a user to specify a logging level for the PHP agent, normally set via the `newrelic.loglevel` PHP ini file.

`-output-dir` : The output-dir flag allows a user to specify a custom folder for the PHP agent log file (`php_agent.log`) and the integration_runner/daemon log file (`integration-tests.log`).

`-pattern` : A shell pattern describing the set of tests that should be run.  The default value runs all the tests. Similarly, `./bin/integration_runner -pattern test_instance*.php` executes any test starting with `test_instance`.

`-php` : The php flag allows a user to specify a custom PHP binary for the integration runner to use when running CLI tests

`-port` : The port flag allows a user to specify which port the integration_runner/daemon will run under.  This is analogous to the `newrelic.daemon.port` setting, normally set in the PHP ini file.

`-retry` : The retry flag allows a user to specify how many times the integration runner will attempt to re-run each individual test.

`-timeout` : The timeout flag allows a user to specify a timeout, in seconds, **for command line tests running under valgrind**. (see `-valgrind` below)

`-valgrind` : The valgrind flag allows a user to point to a valgrind binary file.  If present, and if running a command line (as opposed to web) tests, the integration runner will run the tests using the `ValgrindCLI` type's `Execute` method.

`-threads` : The threads flag allows a user to specify how many individual tests the integration runner will run concurrently using go routines.

PHP Lifecycle
--------------------------------------------------
Another aspect of the integration runner that's important to understand is how it invoked PHP when running its tests cases.

Test cases include the ability to set PHP environment variables via the `/*ENVIORNMENT` directive.  If these variables include a `REQUEST_METHOD`

    #File: tests/integration/output/test_clean_before_header.php
    /*ENVIRONMENT
    REQUEST_METHOD=GET
    */

then the integration runner will treat the test as a *Web* test.  If not, the integration runner will treat the test as a *CLI* test.

For CLI tests, the `integration_runner` will invoke the PHP binary via the Go `os/exec` package.

For Web tests, the integration runner will invoke the `php-cgi` binary that sits next to the configured `php` binary (or via an explicitly set `php-cgi` binary).  The integration runner will use the Go `net/httptest` package to act as a local web server, and use the Go `net/http/cgi` to spawn a CGI process with the `php-cgi` binary.

Understanding this context is important, as PHP can behave differently when invoked via the `php` CLI command vs. when run with the `php-cgi` command.

Also, the integration runner **does not** run the tests in a `mod_php` apache threaded environment, or in a FastCGI `php-fpm` environment.  The integration runner will not catch errors created by subtle differences in these environments.

Test Format
--------------------------------------------------
The integration runner test format is similar, but not identical to, to [the `phpt` test file format used by the PHP core project](https://www.sammyk.me/all-about-phpt-files-writing-tests-for-php-source).  Tests are valid PHP files which the integration runner executes via the `php` or `php-cgi` binaries.  They are identified by their file name and extension

    test_[descriptive name].php

Test writers need to

1. Provide some arbitrary PHP code
2. Using test directives, specify what the expected payloads from the daemon should look like and/or what the output of the test scripts should look like.

Test directives are created in the test files themselves via PHP comments.  There are three broad categories of test directives -- those for configuring the test running environment, those for specifying the expected payloads from the daemon, and those for specifying the expected output of the PHP test itself.

The directive format is

1. A `/*` starting comment
2. The directive name followed by a newline
3. The directive values
4. An ending `*/` comment

For example, the `DESCRIPTION` directive might look like this

    /*DESCRIPTION
    Test describing the intention and caveats of the test case
    */

**Important**: Individual directives may have specific formatting requirements, or their formatting may have semantic value within that specific directive.  Look at existing tests for tips/hints on how to format your directives.


Directives: Configure Environment
--------------------------------------------------
A test writer can use the following directives to configure the environment their tests run in.

`/*ENVIRONMENT`: The ENVIRONMENT directive allows a test writer to provide a set of key/value pairs, which the integration runner will then load as PHP environment variables. (via PHP's `$_SERVER` super global).  Also, if a `REQUEST_METHOD` key is set, the integration runner will consider the test a "web" test, and run the test via the `php-cgi` binary.

`/*HEADERS`: The HEADERS directive only applies to "web" based tests (i.e. those run with the `php-cgi` binary).  This directive allows you to set additional HTTP headers that the integration runner will use when it makes its HTTP request to the go spawned web-server and CGI process that runs the test.

`/*INI`: The INI directive allows you to set PHP ini values, normally set via `php.ini` files, the `php -d` flag, or at runtime via the `ini_set` function.  For command line tests, these ini arguments are set via the `php -d` flag.  For web tests, these ini arguments are set via the `php-cgi -d` flag.

`/*SKIPIF`: The SKIPIF directives allows a test writer to provide a block/snippet of PHP code (within the directive comments -- see other tests for details).  Before running a test, the integration runner will run this PHP snippet in a separate command line context.  If the snippet produces output that begins with the string `skip`, the integration runner will skip this test (and report the test as a skipped test).

By convention, most tests produce the `skip` output by using the `exit` or `die` functions.  However, the integration runner doesn't care about exit codes -- all it cares about is the output of this snippet.  i.e., `echo "skip: skip my test";` will still result in a skipped test.

Finally, while this snippet runs in **its own** context (i.e. even if the test is a web test, the snippet will run as a command line script), the snippet **will** inherit values from the ENVIRONMENT and INI directives.

Directives: Expected Harvest Data
--------------------------------------------------
The intention of most integration tests is to ensure the PHP agent sends the right sort of data to New Relic when a PHP program does certain things.  The `/*EXPECT_ANALYTICS_EVENTS`, `/*EXPECT_CUSTOM_EVENTS`, `/*EXPECT_ERROR_EVENTS`, `/*EXPECT_METRICS`, `/*EXPECT_SLOW_SQLS`, `/*EXPECT_TRACED_ERRORS`, and `/*EXPECT_TXN_TRACES` directives all allow a test writer to specify the data expected for each transaction type.

A test does not need **all** these transaction types.  If you're writing a test to ensure a certain metric is there, you only need to write an `/*EXPECT_METRICS` directive.

The directive/fixture format is not an exact match comparison -- some scrubbing/wildcard features exist. You can copy these from existing tests or (if you're adventurous) by examining [the `comparePayload` function](https://source.datanerd.us/php-agent/php_agent/blob/R7.5/src/newrelic/integration/test.go#L201)

    #File: src/newrelic/integration/test.go
    func (t *Test) comparePayload(expected json.RawMessage, pc newrelic.PayloadCreator, isMetrics bool) {
        /* ... */
    }

The integration runner calls `comparePayload` for [each payload type here](https://source.datanerd.us/php-agent/php_agent/blob/R7.5/src/newrelic/integration/test.go#L316)

    #File: src/newrelic/integration/test.go
    t.comparePayload(t.analyticEvents, harvest.TxnEvents, false)
    t.comparePayload(t.customEvents, harvest.CustomEvents, false)
    t.comparePayload(t.errorEvents, harvest.ErrorEvents, false)
    t.comparePayload(expectedMetrics, harvest.Metrics, true)
    t.comparePayload(t.slowSQLs, harvest.SlowSQLs, false)
    t.comparePayload(t.tracedErrors, harvest.Errors, false)
    t.comparePayload(t.txnTraces, harvest.TxnTraces, false)


Directives: Expected Test Output
--------------------------------------------------
In addition to ensuring the agent sends the correct sort of data, integration tests can also ensure the PHP agent doesn't interfere with the normal running of a PHP program.  That is, there are three directives (`/*EXPECT`, `/*EXPECT_REGEX`, `/*EXPECT_SCRUBBED`) for testing the output of the test script itself.

`/*EXPECT`: The EXPECT directive allows a test write to provide *the exact* output a PHP program (the test script) should produce under test.

`/*EXPECT_REGEX`: The EXPECT_REGEX directive allows a test writer to test the output of a the test against a **Go** regular expression.

`/*EXPECT_SCRUBBED`: The EXPECT_SCRUBBED directive is similar to the EXPECT directive in that it's an "exact match" test, but the integration runner will perform some scrubbing of extraneous test output information before running the comparison.

If you're interested in more details, checkout [the `Compare` function](https://source.datanerd.us/php-agent/php_agent/blob/R7.5/src/newrelic/integration/test.go#L273).

    #File: src/newrelic/integration/test.go
    func (t *Test) Compare(harvest *newrelic.Harvest) {
      if nil != t.expect {
        expect := string(bytes.TrimSpace(t.expect))
        output := string(bytes.TrimSpace(t.Output))

        if expect != output {
          t.Fail(ComparisonFailure{Name: "expect", Expect: expect, Actual: output})
        }
      }

      if nil != t.expectRegex {
        re, err := regexp.Compile(string(t.expectRegex))
        if nil != err {
          t.Fatal(fmt.Errorf("unable to compile expect regex %v: %v", string(t.expectRegex), err))
        } else {
          if !re.Match(t.Output) {
            t.Fail(ComparisonFailure{Name: "regex", Expect: string(t.expectRegex), Actual: string(t.Output)})
          }
        }
      }

      if nil != t.expectScrubbed {
        actual := string(bytes.TrimSpace(ScrubFilename(ScrubLineNumbers(t.Output), t.Path)))
        expect := string(bytes.TrimSpace(t.expectScrubbed))

        if expect != actual {
          t.Fail(ComparisonFailure{Name: "scrubbed", Expect: expect, Actual: actual})
        }
      }

Directives: Misc
--------------------------------------------------
There's a few extra directive that don't fit nicely into any one category.

`/*EXPECT_HARVEST`: The EXPECT_HARVEST directive allows a test writer to tell the integration runner that no harvest is expected by supplying a value of `no`

```
/*EXPECT_HARVEST
no
*/
```

This is useful in testing cases where we expect the agent to be disabled or ignored.

`/*DESCRIPTION`: The DESCRIPTION directive is neither a configuration directive, or a test directive.  This is meant for humans to read and understand the context/scope/intention of a test case.

`/*XFAIL`: The XFAIL directive allows a test writer to mark a test as e**X**pected to fail.  A XFAIL directive should contain descriptive test describing why the test writer expects the test to fail.

**IMPORTANT**: The integration runner currently doesn't consider an xfail test that passes a "bad" test run.  i.e. If a test marked `XFAIL` actually passes, the test runner will still report an `xfail` result.

## TAP Test Assertions
--------------------------------------------------
The integration test suite also includes a simple implementation of the [Test Anything Protocol (TAP)](http://testanything.org/tap-specification.html).  Test writers can use the assertion functions defined in [`tests/include/tap.php`](https://source.datanerd.us/php-agent/php_agent/blob/12abccaf886dd025ebc484e16f8d5559b07019a5/tests/include/tap.php) to assist in writing their tests

    <?php
    /*DESCRIPTION
    A simple example of the tap functions.
    */
    /*EXPECT
    ok - Is 6 * 7 equal to 42?
    */
    // the tap library is located in the top level `tests/include` folder,
    // which means searching upwards from the test file's folder via `..`
    require_once(realpath (dirname ( __FILE__ )) . '/../../include/tap.php');

    $theAnswer = 6 * 7;
    tap_equal(42, $theAnswer, "Is 6 * 7 equal to 42?");

**IMPORTANT**: The integration runner will not automatically detect output form the `tap_` functions.  It's up to the test writers to put the expected output in the `/*EXPECT` directive.
