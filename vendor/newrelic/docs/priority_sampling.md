# Priority Sampling in the PHP Agent

The PHP Agent uses three finite event sampling pools to store events between
harvests; these are the custom events, transaction events, and the error 
events sampling pools. When the limits for each pool are reached, events are
replaced according to a [priority sampling algorithm](https://source.datanerd.us/agents/agent-specs/blob/master/Priority-Sampling.md).  
The priority sampling algorithm uses a randomly-generated priority for
transactions and events to make its sampling decisions. This document describes
the approach used by the PHP Agent and Daemon to generate, store, communicate 
and leverage priority for transactions and events.

On the PHP Agent side, each time a new transaction is begun using 
`nr_txn_begin()`, a `sampling_priority` between 0.0 and 1.0, or `[0.0, 1.0)` 
is randomly generated and stored in the corresponding `struct _nrtxn_t`.  Each 
transaction's `sampling_priority` is communicated to the PHP Daemon by flatbuffer
in two places. First, it's a field in the `Transaction` table, `sampling_priority`,
as specified in `protocol.fbs`. Second, it's part of the precomputed JSON
for multiple kinds of events. For example, for the Transaction Event, the 
sampling priority is part of the intrinsics, as converted by 
`nr_txn_event_intrinsics()`. The first location for priority is used by the PHP
Daemon for sampling decisions. The second location is passed through the PHP
Daemon, communicated to New Relic as part of BetterCAT.

But a transaction is not the only data type that requires a priority. To make
sampling decisions for each of the three event pools, the PHP Daemon needs a
priority for every event. For these sampling decisions, each event type in 
the PHP Daemon -- e.g., `ErrorEvent` or `CustomEvent` --  has a `SamplingPriority`
field that is populated by the `sampling_priority` field from the flatbuffer.
The PHP Daemon then uses that value to make sampling decisions for those pools.

## When Priority Sampling Isn't Used in the PHP Agent

There are two moments where priority sampling is not used by the PHP Agent.

**Error**. Each transaction instrumented by the agent saves a single error to
report alongside the transaction. Errors are associated with a default priority
or a priority that may be set by the user in the API call 
`newrelic_notice_error()`.  It is important to note that this priority is *not*
the same as the sampling priority described in the previous section. For more 
information, see the function `nr_txn_record_error()` in `nr_txn.c` in the 
`axiom` library.

**Custom Event**. Each transaction maintains a pool of custom events that occur
during the transaction. If more than 10,000 events occur during a transaction,
they are sampled by the PHP Agent using random sampling. For more information,
see `nr_analytics_events.c` in the `axiom` library. 

As noted above, custom events are reported to the PHP Daemon by each PHP Agent;
the PHP Daemon samples these events using priority sampling.

## When Priority Sampling Isn't Used in the PHP Daemon

For data types that are not events -- e.g., `Error` or `SlowSQL` -- the PHP
Daemon uses different approaches to sampling.  These are:

**Error**. As in the PHP Agent, errors are sampled according to their error
priority.

**Slow SQL**.  The PHP Daemon samples Slow SQLs according to their speed.
The fastest Slow SQLs are replaced when the pool of Slow SQLs has reached 
its size limit.

**Transaction Trace**.  The PHP Daemon samples Transaction Traces according
to their duration.  Traces with the smallest duration are replaced when the
pool of traces has reached its size limit.