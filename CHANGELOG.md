# New Relic C Agent Release Notes #

## Master Branch (In Progress/Current Working Version) ##

### New Features ###

### Bug Fixes ###

### End of Life Notices ###

### Upgrade Notices ###

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

### End of Life Notices ###

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

### Notes ###

### Internal Changes ###
