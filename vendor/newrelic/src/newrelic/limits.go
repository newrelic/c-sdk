package newrelic

import (
	"time"
)

const (
	// AppLimit is the maximum number of applications that the daemon will
	// support.  The agent's limit is in nr_app.h.
	AppLimit                 = 250
	AppConnectAttemptBackoff = 30 * time.Second

	// DefaultAppTimeout specifies the elapsed time after which an application
	// will be considered inactive. Inactive applications have their agent runs
	// dropped and require a reconnect to being collecting data again.
	DefaultAppTimeout = 10 * time.Minute

	// Harvest Data Limits

	MaxMetrics            = 2 * 1000
	MaxTxnEvents          = 10 * 1000
	MaxCustomEvents       = 10 * 1000
	MaxErrorEvents        = 100
	MaxSpanEvents         = 1000
	MaxErrors             = 20
	MaxSlowSQLs           = 10
	MaxRegularTraces      = 1
	MaxForcePersistTraces = 10
	MaxSyntheticsTraces   = 20

	// Failed Harvest Data Rollover Limits
	// Use the same harvest failure limit for custom events and txn events

	FailedEventsAttemptsLimit = 10
	FailedMetricAttemptsLimit = 5

	// MaxPidfileRetries is the maximum number of attempts the daemon
	// will make to acquire exclusive access to a pid file before returning
	// an error.
	MaxPidfileRetries = 10

	// MaxOutboundConns is the maximum number of concurrent connections
	// to New Relic.
	MaxOutboundConns = 100

	// HarvestTimeout specifies a time limit for requests made to send
	// harvest data to the collector. This timeout includes connection
	// time. It also specifies the maximum time limit to wait for a
	// chance to perform the request. This value should be less than
	// 60 seconds so that data can be merged into the next harvest if
	// the time limit is exceeded.
	HarvestTimeout = 45 * time.Second

	// Processor channel buffering:

	TxnDataChanBuffering = 10000
	AppInfoChanBuffering = 100

	// HostLengthByteLimit is the maximum number of bytes that will be
	// sent for either the host and display host names in the connect command.
	HostLengthByteLimit = 255

	// LabelNumberLimit is the maximum number of labels that will be sent.
	LabelNumberLimit = 64

	// MinFlatbufferSize is the minimum size of a flatbuffers message (no agent
	// run or message body). This should be updated when new fields are added.
	MinFlatbufferSize = 12
)
