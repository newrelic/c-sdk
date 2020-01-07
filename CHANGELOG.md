# New Relic C Agent Release Notes #

## Contents ##

| Release Number | Release Date |
| ------------- |-------------|
| [1.3.0](#130) | 2020-01-07 |
| [1.2.0](#120) | 2019-10-09 |
| [1.1.0](#110) | 2019-06-04 |
| [1.0.1](#101) | 2019-05-02 |
| [1.0.0](#100) | 2019-04-23 |
| [0.1.3](#013) | 2019-03-20 |
| [0.1.2](#012) | 2019-02-22 |
| [0.1.1](#011) | 2019-01-25 |
| [0.1.0](#010) | 2018-12-21 |
| [0.0.6](#006) | 2018-08-10 |
| [0.0.5](#005) | 2018-03-12 |
| [0.0.4](#004) | 2018-02-12 |
| [0.0.3](#003-alpha) | 2018-01-16 |
| [0.0.2](#002-alpha) | 2017-10-30 |
| [0.0.1](#001-alpha) | 2017-09-21 |

## 1.3.0 ##

### Bug Fixes ###

- A default configuration created with `newrelic_create_app_config()` will now
  work as expected with license keys for non-US accounts.

- For inbound distributed tracing payloads with invalid or missing values for
  `pr` (priority) and/or `sa` (sampled) the agent used to assign a default
  priority of -1 and/or a default sampled value of `false` to the transaction.

  This has been fixed, the agent now keeps initial priority and sampled values
  if the respective values in the inbound distributed tracing payload are
  missing or invalid.

### New Features ###

- Event data is now sent to New Relic every five seconds, instead of every
  minute. As a result, transaction, custom, and error events will appear in
  near-realtime within APM and Insights.

  Note that the overall limits on how many events can be sent per minute have
  not changed. Also, metric and trace data is unaffected, and will still be
  sent every minute.o

### Internal Changes ###

- An error when compiling the test suite with GCC 9.2 was fixed.

## 1.2.0 ##

### New Features ###

**More flexibility for container deployments**

- The daemon and the instrumented application no longer have to reside on the
  same host and can now communicate over an IPv4 or IPv6 TCP socket. For the
  instrumented application this can be configured via the `daemon_socket`
  argument of the `newrelic_init` function. For the daemon this can be
  configured via the `--address` command line option.
- When terminating the daemon via the `SIGTERM` signal (and/or the `SIGINT`
  signal if started with the `-f`, `--foreground` flag), the daemon will now
  send all buffered data to New Relic prior to exiting.
- The daemon has introduced a new configuration `--watchdog-foreground`. This
  keeps the daemon watchdog process in the foreground, whereas the
  `--foreground` configuration keeps the daemon worker process in the
  foreground. The new configuration makes it possible to use the daemon in a
  blocking way, without losing the additional stability provided by the watchdog
  process.
- The C SDK now offers `newrelic_set_transaction_name` with which users may
  change the name of an already started transaction.

### Bug Fixes ###

- The daemon now synchronously handles critical code paths related to harvesting
  and merging transaction data. This prevents crashes caused by race conditions.

### Upgrade Notices ###

- The daemon has introduced a new configuration `--address` which serves as an
  alias to `--port`. Customers may use either to specify the location of the
  daemon. If both values are set, `--address` takes precedence.

* When starting the daemon, it will now wait for up to three seconds for the
  listening port to be ready to receive connections before forking into the
  background. This usually occurs in (much) less than a second, and most users
  with this configuration will notice no difference in practice.

  The time that the daemon will wait can be controlled by setting the
  `--wait-for-port` setting with a duration. This duration may be 0 to prevent
  any blocking. If the option is omitted, the default value is `3s`.

  Daemons started in foreground mode (with the `--foreground` flag) are
  unaffected, and will behave as before.

## 1.1.0 ##

### New Features ###

- **Support for Distributed tracing**

    Distributed tracing lets you see the path that a request
    takes as it travels through your distributed system. By
    showing the distributed activity through a unified view,
    you can troubleshoot and understand a complex system better
    than ever before.

    Distributed tracing is available with an APM Pro or
    equivalent subscription. To see a complete distributed
    trace, you need to enable the feature on a set of
    neighboring services.

    Refer to the New Relic
    [C SDK documentation](https://docs.newrelic.com/docs/agents/c-sdk/instrumentation/enable-distributed-tracing-your-c-applications)
    for more information on enabling distributed tracing for the C SDK.

### End of Life Notices ###

- The `log_filename` and `log_level` fields of `newrelic_app_config_t` are
  currently deprecated.  Future versions of the C SDK permanently remove these fields.
  To specify the log file and log level to use with the C SDK, use
  `newrelic_configure_log()`.

## 1.0.1 ##

### Upgrade Notices ###

- The `newrelic_process_config_t` struct was erroneously left in
  `libnewrelic.h` when the functionality that used it was refactored into
  `newrelic_init()`. The struct has been removed.

  Although this technically constitutes a backward compatibility break, we have
  decided to do this in a minor release as there was no actual way to use the
  struct with the API in version 1.0.

## 1.0.0 ##

This is the first release of the New Relic C SDK! If your application does not use
other New Relic APM agent languages, you can use the C SDK to take advantage of
New Relic's monitoring capabilities and features to instrument a wide range of
applications.

For more information, see:

[Documentation](https://docs.newrelic.com/docs/c-sdk-table-contents): How to get
started with the C SDK, install and configure it, instrument transactions,
segments, and errors, use the C SDK API, and do some basic troubleshooting.

### End of Life Notice ###

The previous APM Agent SDK is deprecated beta software. If you were previously
using the Agent SDK, you can switch to the C SDK. Check the
[compatibility and requirements](https://docs.newrelic.com/docs/c-sdk-compatibility-requirements),
and then
[instrument, compile and link your application's code](https://docs.newrelic.com/docs/install-c-sdk-compile-link-your-code)
to use the C SDK.  The C SDK currently does not support New Relic's HSM feature;
this may impact how you schedule your transition away from the Agent SDK.

## 0.1.3 ##

### Bug Fixes ###

- Calculation of transaction total time and metric exclusive time in an
  asynchronous context has been improved, which should prevent issues resulting
  in broken transaction charts in APM.

- Rollup datastore and external segment metrics, used to display application
  overview charts in APM, have been improved.

## 0.1.2 ##

### New Features ###

- The C agent now offers `newrelic_set_transaction_timing()` with which users
  may manually change transaction timing. Though the C agent is incredibly
  effective at automatically timing transactions, providing users with this
  kind of timing allows them to achieve consistent timing values across
  the New Relic Platform and their own internal monitoring systems.

- The C agent now offers `newrelic_set_segment_parent_root()` with which users
  may manually reparent a segment with the transaction's root segment; this is
  useful in scenarios in which an asynchronous segment is best represented
  as a top-level call in the transaction.

### Upgrade Notices ###

- Application configuration is now completely separate from global
  configuration.

  In practice, this means that uses of `newrelic_config_t` and calls to
  `newrelic_new_config()` should be replaced with `newrelic_app_config_t` and
  `newrelic_create_app_config()`, respectively.

  Logging and daemon socket configuration should now be done by invoking
  `newrelic_configure_log()` and `newrelic_init()`, respectively.
  `newrelic_init()` _must_ be invoked before the first application is created,
  otherwise default values will be used.

- `newrelic_loglevel_t` constant names are now prefixed with `NEWRELIC_`.

## 0.1.1 ##

### New Features ###

- Custom events can now be sent using the new Custom Events API.  Events
  are created via the `newrelic_create_custom_event` function, and attributes
  may be added to custom events via the `newrelic_custom_event_add_*` family
  of functions.  See GUIDE.md for usage information.

- Custom metrics can now be sent using the new Custom Metrics API.
  See GUIDE.md for usage information.

## 0.1.0 ##

### New Features ###

- Segments may now be reparented manually using the new
  `newrelic_set_segment_parent()` function.

- Segments now support manual timing via the new
  `newrelic_set_segment_timing()` function.

### Upgrade Notices ###

- The segment API has been mildly refactored: all segment types are now
  represented as `newrelic_segment_t`, and the `newrelic_datastore_segment_t`
  and `newrelic_external_segment_t` types have been removed.

  Similarly, all segments are now ended with `newrelic_end_segment()`, and
  therefore the `newrelic_end_datastore_segment()` and
  `newrelic_end_external_segment()` functions have been removed.

## 0.0.6 ##

### New Features ###

- Added support for creating custom segments using the new
  `newrelic_start_segment()` and `newrelic_end_segment()` functions. See
  `libnewrelic.h`, `GUIDE.md` and `examples/ex_segment.c` for usage
  information.

## 0.0.5 ##

### Upgrade Notices ###

- To increase security, TLS will now always be used in communication between
  the C Agent and New Relic servers.  This change should be invisible to
  customers since the C Agent did not previously offer any way to disable TLS.

## 0.0.4 ##

### New Features ###

- Added support for creating datastore segments using the new
  `newrelic_start_datastore_segment()` and `newrelic_end_datastore_segment()`
  functions. See `libnewrelic.h`, `GUIDE.md` and `examples/ex_datastore.c` for usage
  information.

- Added support for creating external segments using the new
  `newrelic_start_external_segment()` and `newrelic_end_external_segment()`
  functions. See `libnewrelic.h`, `GUIDE.md` and `examples/ex_external.c` for usage
  information.

- Added configuration options to control transaction trace generation.

## 0.0.3-alpha ##

### New Features ###

- Customers may now use `newrelic_notice_error()` to record transaction errors that
  are not automatically handled by the agent. Errors recorded in this manner are displayed in
  [error traces](https://docs.newrelic.com/docs/apm/applications-menu/error-analytics/error-analytics-explore-events-behind-errors#traces-table)
  and are available to query through
  [New Relic Insights](https://docs.newrelic.com/docs/insights/use-insights-ui/getting-started/introduction-new-relic-insights).
  See `libnewrelic.h` for usage information.

### Bug Fixes ###

- At times, when the daemon removed an application after a 10-minute timeout, the agent
  daemon exited in failure. This has been fixed.

### Upgrade Notices ###

* The function `bool newrelic_end_transaction(newrelic_txn_t** transaction)` has changed
  its return type from void to bool. A value of true is returned if the transaction was
  properly sent. An error is still logged on failure.

* The function `bool newrelic_destroy_app(newrelic_app_t** app)` has changed its return
  type from void to bool.

## 0.0.1-alpha ##

### New Features ###

- Initial Release

### Bug Fixes ###

- Initial Release, no bug fixes
