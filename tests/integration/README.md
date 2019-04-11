# C SDK integration tests
The purpose of the integration runner is to test the data transmitted by the C SDK API calls to the daemon. The integration runner mimics enough of the behaviour of the daemon to yield successful communication between it and an application instrumented with the C SDK. The tests evaluate whether the expected data was communicated to the daemon.

The tests found in the `tests/integration` directory are run by the integration runner. This integration runner code is written in Go and reuses components of the daemon. The purpose of the integration tests is to ensure that the sdk component C SDK is packaging and sending data to the daemon correctly.

When the integration runner starts it performs a handshake with the backend to validate the application and grab New Relic provided configuration variables. Next, it iterates over the test_*.c files and executes each test case. During this time, the integration runner accepts requests from the C SDK. However, it does not perform a harvest cycle, and does not sent information to the New Relic backend.

Instead of reporting data to New Relic, the integration runner instead examines the data reported to it by each test case. It compares this data to the expected values specified in each test case file. It reports ok, not ok, skip, or xfail (expected fail) for each test.

If you're a fan of listed steps, here's another way of saying the above.

1. The integration runner compiles the test code.
1. The test case code runs
1. The C SDK extension collects instrumentation as the code runs
1. The C SDK extension sends instrumentation to the integration-runner-acting-as-daemon
1. The integration-runner-acting-as-daemon holds on to the data 


# Table of Contents
1. [Running the integration tests](#running-the-integration-tests)
    1. [Integration Runner CLI Flags](#integration-runner-cli-flags)
1. [Writing Integration Tests](#writing-integration-tests)
    1. [Test Format](#test-format)
        * [Directives](#directives)
    1. [Write tests using predefined helper macros](#write-tests-using-predefined-helper-macros)
    1. [Write tests by providing your own `main` function](#write-tests-by-providing-your-own-main-function)

## Running the integration tests
Running the integration tests is as simple as running the `make integration` command in the top level directory. This command will point the integration runner at the test/integration directory then make and run all files that match the pattern: `test_*.c`. The Make command **requires** that a Makefile be present in the same directory as the `test_*.c` file. An example of this can be found [here](custom_metric/Makefile).


### Integration Runner CLI Flags

The integration runner accepts a number of optional command line flags.
Here's a brief description of each flag -- if you're curious about specific implementation details these flags are parsed in [`main.go`](../../vendor/newrelic/src/integration_runner/main.go).

Flag | Description
---|---
`-loglevel` | The loglevel flag allows a user to specify a logging level for the C SDK , normally set via a call to `newrelic_configure_log()`.
`-output-dir` | The output-dir flag allows a user to specify a custom folder for the C SDK log file (`c_agent.log`) and the integration_runner/daemon log file (`integration-tests.log`).
`-pattern` | A shell pattern describing the set of tests that should be run.  The default value runs all the tests. Similarly, `./bin/integration_runner -pattern test_instance*.c` executes any test starting with `test_instance`.
`-port` | The port flag allows a user to specify which port the integration_runner/daemon will run under.
`-retry` | The retry flag allows a user to specify how many times the integration runner will attempt to re-run each individual test.
`-timeout` | The timeout flag allows a user to specify a timeout, in seconds, **for command line tests running under valgrind**. (see `-valgrind` below)
`-valgrind` | The valgrind flag allows a user to point to a valgrind binary file.  If present, and if running a command line (as opposed to web) tests, the integration runner will run the tests using the `ValgrindCLI` type's `Execute` method.
`-threads` | The threads flag allows a user to specify how many individual tests the integration runner will run concurrently using go routines.


<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

## Writing Integration Tests

### Test Format
Test files must be written in a specific format for the integration runner to function properly. This section will describe this format. The test file name needs to follow the pattern of `test_*.c`.

Test directives are created in the test files via comments. There are three broad categories of test directives -- those for configuring the test running environment, those for specifying the expected payloads from the daemon, and those for specifying the expected output of the test itself.

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

<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

#### Directives

* [`DESCRIPTION`](#the-description-directive)
* [`CONFIG`](#the-config-directive)
* [`EXPECT_ANALYTICS_EVENTS`](#expect_analytics_events)
* [`EXPECT_CUSTOM_EVENTS`](#expect_custom_events)
* [`EXPECT_ERROR_EVENTS`](#expect_error_events)
* [`EXPECT_SPAN_EVENTS` ](#expect_span_events)
* [`EXPECT_METRICS`](#expect_metrics)
* [`EXPECT_TRACED_ERRORS`](#expect_traced_errors)
* [`EXPECT_TXN_TRACES`](#expect_txn_traces)

##### The `DESCRIPTION` directive
The purpose of the description section is to do just that, describe the test. This section is used as a short comment about a file's purpose.

##### The `CONFIG` directive
The `CONFIG` comment block must contain valid C code that manipulates a
`newrelic_app_config_t* cfg` data structure. Here's an example:

```c
/*CONFIG
  cfg->transaction_tracer.threshold = NEWRELIC_THRESHOLD_IS_OVER_DURATION;
  cfg->transaction_tracer.duration_us = 1;
*/
```

This code is injected in the test executable. This input is not scrubbed, no additional checks are done and there is no preprocessing or sandboxing. 

##### The `EXPECT_*` directives

There are a number of directives that begin with the the word `EXPECT`. All of these directives are used to test output of some kind. There are several that are used to test the output of the test code itself and many to test the output of the C SDK. The output directives can also ensure the C SDK doesn't interfere with normal operations of the monitored application. The directives that test the output of the test code are:

* `EXPECT` Expect tests that the output matches the what's in the expect block exactly.
* `EXPECT_REGEX` The EXPECT_REGEX allows the use of [**Go** regular expressions](https://github.com/google/re2/wiki/Syntax). 

There are also directives that test the instrumentation output of the C SDK. Wildcards can be added to the expected output with double question marks: `??`. You can add a Go style regex to any of the EXPECT_* Directives by starting with a `/` and ending with a `/`. Here is an example, with the escape characters: `\/[abc]\/`. These are separated by types of output. The exhaustive list is below, along with more information about each:

###### `EXPECT_ANALYTICS_EVENTS`

A transaction events payload is an array of transaction events. Transaction events map to a single web transaction observed by the agent.
```
/*EXPECT_ANALYTICS_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": "??"
  },
  [
    [
      {
        "type": "Transaction",
        "name": "OtherTransaction\/php__FILE__",
        "timestamp": "??",
        "duration": "??",
        "totalTime": "??",
        "error": false
      },
      {
        "user_parameter":"foo",
      },
      {
        "this_is_an": "Agent Attribute!",
        "zip": "zap"
      }
    ]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_CUSTOM_EVENTS`
If you would like to learn more about custom events visit the [Guide.md](../../blob/master/GUIDE.md#creating-custom-events).
```
/*EXPECT_CUSTOM_EVENTS
[
  "?? agent run id",
  "?? sampling information",
  [
    [
      {
        "type":"Some Name",
        "timestamp":"??"
      },
      {
        "a-string": "Forty Two",
        "a-double": 42.40
      },
      {}
    ]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_ERROR_EVENTS`
The event format is identical to analytics events.
```
/*EXPECT_ERROR_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": 1
  },
  [
    [
      {
        "type": "TransactionError",
        "timestamp": "??",
        "error.class": "Exception",
        "error.message": "High",
        "transactionName": "OtherTransaction/Action/basic",
        "duration": "??",
        "queueDuration": "??",
        "nr.transactionGuid": "??"
      },
      {
        "user":"someone",
        "costom_id":22
      },
      {
        "httpResponseCode": 500,
        "request.headers.method": "GET"
      }
    ]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_SPAN_EVENTS` 

Spans are used with distributed tracing, you can read more [here](https://docs.newrelic.com/docs/apm/distributed-tracing/ui-data/span-event). The structure of the span event json can be seen in the code that generates it [here](../../php_agent/axiom/cmd_txndata_transmit.c#l103-l149).
 
 
 Example:
```
/*EXPECT_SPAN_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": 1000,
    "events_seen": 1
  },
  [
    [
      {
        "traceId": "??",
        "duration": "??", 
        "transactionId": "??",
        "name": "OtherTransaction\/php__FILE__",
        "guid": "??",
        "type": "Span",
        "category": "generic",
        "priority": "??", 
        "sampled": true,
        "nr.entryPoint": true,
        "timestamp": "??",
        "parentId" : "4321"
      },
      {},
      {}
    ]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_METRICS`
When using `EXPECT_METRICS` you must include all the metrics you expect to see. The metrics are stored as an array and have the following format:
```
[{“name”:”name of metric”, 
  “scope”:”scope of metric”}, [count, total time, exclusive time, min time, max time, sum of squares]]
```
 You can read about custom metrics [here](../../blob/master/GUIDE.md#custom-metrics). This example shows that when we are only testing for the count on each metric. We use `??` wildcards to skip the other values.
```
/*EXPECT_METRICS
[
  "?? agent run id",
  "?? start time",
  "?? stop time",
  [
    [{"name":"OtherTransaction/all"},                        [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                   [1, "??", "??", "??", "??", "??"]]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_TRACED_ERRORS`
A traced error represents a detailed view of one error. Each traced error is represented as a JSON array. Unlike the rest of the error trace JSON (which is formatted as an array of values), the error trace attributes use named key-value pairs.
```
[
    timestamp,
    transactionName,
    message,
    errorType,
    {
        "stack_trace": "",
        "agentAttributes": {},
        "userAttributes": {},
        "intrinsics": {}
    },
    catGUID
]
```
Example:
```
/*EXPECT_TRACED_ERRORS
[
  "?? agent run id",
  [
    [
      "?? when",
      "OtherTransaction/Action/basic",
      "High",
      "Exception",
      {
        "stack_trace": "??",
        "agentAttributes": "??",
        "intrinsics": "??"
      }
    ]
  ]
]
*/
```

<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

###### `EXPECT_TXN_TRACES`
Each trace is represented as a nested JSON array.

Example:
```
/*EXPECT_TXN_TRACES
[
  "?? agent run id",
  [
    [
      "?? entry",
      "?? duration",
      "OtherTransaction/Action/basic",
      "<unknown>",
      [
        [
          0, {}, {},
          [
            "?? start time", "?? end time", "ROOT", "?? root attributes",
            [
              [
                "?? start time", "?? end time", "`0", "?? node attributes",
                [
                  [
                    "?? start time", "?? end time", "`1",
                    {
                      "uri": "http://example.com",
                      "library": "curl",
                      "procedure": "GET"
                    },
                    []
                  ]
                ]
              ]
            ]
          ],
          {
            "intrinsics": {
              "totalTime": "??",
              "cpu_time": "??",
              "cpu_user_time": "??",
              "cpu_sys_time": "??"
            }
          }
        ],
        [
          "OtherTransaction/Action/basic",
          "External/example.com/all"
        ]
      ],
      "?? txn guid",
      "?? reserved",
      "?? force persist",
      "?? x-ray sessions",
      null
    ]
  ]
]
*/
```
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

##### The `XFAIL` directive
The XFAIL directive is used to mark a test as ignored. The test should describe why it's ignored.

**IMPORTANT**: The integration runner currently doesn't consider an xfail test that passes a "bad" test run.  i.e. If a test marked `XFAIL` actually passes, the test runner will still report an `xfail` result.


### Write tests using predefined helper macros

C Agent integration tests can include the file `common.h`. This file provides a
`main` function as well as some macros that should avoid repetitive boilerplate
code in tests.

The following macros are provided:

* `RUN_APP()`: Initializes a `newrelic_app_t*` and provides it as `app` in the
  code block following the macro.
* `RUN_WEB_TXN(M_txnname)`: In addition to providing `app` as does `RUN_APP()`,
  this also provides an initialized `newrelic_txn_t *` web transaction named
`txn`. The string given as `M_txnname` is the transaction name.
* `RUN_NONWEB_TXN(M_txnname)`: In addition to providing `app` as does `RUN_APP()`,
  this also provides an initialized `newrelic_txn_t *` non-web transaction named
`txn`. The string given as `M_txnname` is the transaction name.

In each test file only one of those macros can be used.

By default an agent log file `./c_sdk.log` is created. This path can be
changed by setting the environment variable `NEW_RELIC_LOG_FILE`.
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>

### Write tests by providing your own `main` function

Tests can also be written providing a custom `main` function. In this case
`common.h` must not be included.

When using this approach, the following compile time defines are important:

* `NEW_RELIC_CONFIG`: The contents of the `CONFIG` comment block.
* `NEW_RELIC_DAEMON_TESTNAME`: The name of the test. This has to be used as
  application name, otherwise the integration runner cannot properly assign
  harvests to test cases.

In addition, the integration runner sets the following environment variables at
test runtime:

* `NEW_RELIC_LICENSE_KEY`: The license key used by the integration runner.
* `NEW_RELIC_DAEMON_SOCKET`: The socket on which the integration runner
  accepts connections.
  
<div align="right">
    <b><a href="#table-of-contents">↥ back to the table of contents</a></b>
</div>
