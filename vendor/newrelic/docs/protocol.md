# Protocol

Communication between the agent and the daemon is always initiated by the agent.
Depending on the message type, the daemon may or may not respond with a reply.

Much of the relevant code is contained in [listener.go](../src/newrelic/listener.go) and
[commands.go](../src/newrelic/commands.go).

## Format

#### Preamble

All messages are prefixed by an 8-byte preamble:

1.  4-byte integer containing the message length (excluding the preamble) in
    little endian format.
2.  4-byte integer containing the format constant little endian format.  This
    reserved field will allow future versioning or future support on non-
    FlatBuffers message bodies.

#### Body

Currently, all message bodies contain FlatBuffers.

The specification file is [src/newrelic/protocol.fbs](../src/newrelic/protocol.fbs)

## Expected Agent Behavior

The agent begins by issuing an `App` message to the daemon.  The daemon will
respond with an `AppReply`.

If the app state returned by the daemon in response to the `App` message is
`InvalidLicense` or `Disconnected`, the agent should not communicate with the
daemon again (for anything pertaining to that app).

If the app state returned by the daemon is `Unknown`, the agent should continue
to retry the `App` query every couple of seconds. The `Unknown` status means
that the daemon has not seen the app before or has not completed connecting it
to the collector.

If the app state returned by the daemon is `Connected`, then the reply will also
contain the full server settings from the collector in the `connect_reply`
field, including the `agent_run_id`. The agent should apply these settings, and
save the `agent_run_id` for future use.

Once the daemon replies that the app is `Connected`, the agent may begin
instrumentation and start sending transaction data to the daemon.  The
`Transaction` message is always sent with the `agent_run_id` originally received
with the response to the `App` message.  The daemon does not respond to a this
message.  It is assumed that the agent will send a single `Transaction` message
to the daemon at the end of every transaction: it is not designed to contain
data from multiple transactions.

As long as an application is in a `Connected` state, the agent must continue to
perform an `App` query every `NR_APP_REFRESH_QUERY_PERIOD_SECONDS` seconds to
ensure that the agent run remains valid. This is because the application may be
restarted (with new settings) or disconnected.  When doing so, the agent should
provide the current `agent_run_id` as part of the request payload.  This will
allow the daemon to make an abbreviated `AppReply` if the app is unchanged, thus
avoiding the large `connect_reply` field.

Note that this behavior means that if the app is restarted, it may take up to
`NR_APP_REFRESH_QUERY_PERIOD_SECONDS` seconds for all agents to get the latest
settings.  This is OK: the advantage of this system over one where every message
gets a reply is that the number of replies is quite limited.  The PHP agent is
single threaded, and therefore waiting for a reply stops the agent process from
handling requests.

