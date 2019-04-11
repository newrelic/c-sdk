package newrelic

import (
	"time"

	"newrelic/collector"
)

type AggregaterInto interface {
	AggregateInto(h *Harvest)
}

type Harvest struct {
	Metrics           *MetricTable
	Errors            *ErrorHeap
	SlowSQLs          *SlowSQLs
	TxnTraces         *TxnTraces
	TxnEvents         *TxnEvents
	CustomEvents      *CustomEvents
	ErrorEvents       *ErrorEvents
	SpanEvents        *SpanEvents
	commandsProcessed int
	pidSet            map[int]struct{}
}

func NewHarvest(now time.Time) *Harvest {
	return &Harvest{
		Metrics:           NewMetricTable(MaxMetrics, now),
		Errors:            NewErrorHeap(MaxErrors),
		SlowSQLs:          NewSlowSQLs(MaxSlowSQLs),
		TxnTraces:         NewTxnTraces(),
		TxnEvents:         NewTxnEvents(MaxTxnEvents),
		CustomEvents:      NewCustomEvents(MaxCustomEvents),
		ErrorEvents:       NewErrorEvents(MaxErrorEvents),
		SpanEvents:        NewSpanEvents(MaxSpanEvents),
		commandsProcessed: 0,
		pidSet:            make(map[int]struct{}),
	}
}

func (h *Harvest) empty() bool {
	return len(h.pidSet) == 0 &&
		h.CustomEvents.Empty() &&
		h.ErrorEvents.Empty() &&
		h.SpanEvents.Empty() &&
		h.Errors.Empty() &&
		h.Metrics.Empty() &&
		h.SlowSQLs.Empty() &&
		h.TxnEvents.Empty() &&
		h.TxnTraces.Empty()
}

func (h *Harvest) createFinalMetrics() {
	if h.empty() {
		// No agent data received, do not create derived metrics. This allows
		// upstream to detect inactivity sooner.
		return
	}

	pidSetSize := len(h.pidSet)

	if 0 == pidSetSize {
		// For UI purposes, Instance/Reporting has to be nonzero.
		pidSetSize = 1
	}

	// NOTE: It is important that this metric be created once per minute.
	h.Metrics.AddCount("Instance/Reporting", "", float64(pidSetSize), Forced)

	// Custom Events Supportability Metrics
	h.Metrics.AddCount("Supportability/Events/Customer/Seen", "", h.CustomEvents.NumSeen(), Forced)
	h.Metrics.AddCount("Supportability/Events/Customer/Sent", "", h.CustomEvents.NumSaved(), Forced)

	// Transaction Events Supportability Metrics
	// Note that these metrics used to have different names:
	//   Supportability/RequestSampler/requests
	//   Supportability/RequestSampler/samples
	h.Metrics.AddCount("Supportability/AnalyticsEvents/TotalEventsSeen", "", h.TxnEvents.NumSeen(), Forced)
	h.Metrics.AddCount("Supportability/AnalyticsEvents/TotalEventsSent", "", h.TxnEvents.NumSaved(), Forced)

	// Error Events Supportability Metrics
	h.Metrics.AddCount("Supportability/Events/TransactionError/Seen", "", h.ErrorEvents.NumSeen(), Forced)
	h.Metrics.AddCount("Supportability/Events/TransactionError/Sent", "", h.ErrorEvents.NumSaved(), Forced)

	if h.Metrics.numDropped > 0 {
		h.Metrics.AddCount("Supportability/MetricsDropped", "", float64(h.Metrics.numDropped), Forced)
	}

	// Span Events Supportability Metrics
	h.Metrics.AddCount("Supportability/SpanEvent/TotalEventsSeen", "", h.SpanEvents.analyticsEvents.NumSeen(), Forced)
	h.Metrics.AddCount("Supportability/SpanEvent/TotalEventsSent", "", h.SpanEvents.analyticsEvents.NumSaved(), Forced)
}

type FailedHarvestSaver interface {
	FailedHarvest(*Harvest)
}

type PayloadCreator interface {
	FailedHarvestSaver
	Empty() bool
	Data(id AgentRunID, harvestStart time.Time) ([]byte, error)
	// For many data types, the audit version is the same as the data. Those
	// data types return nil from Audit.
	Audit(id AgentRunID, harvestStart time.Time) ([]byte, error)
	Cmd() string
}

func (x *MetricTable) Cmd() string  { return collector.CommandMetrics }
func (x *CustomEvents) Cmd() string { return collector.CommandCustomEvents }
func (x *ErrorEvents) Cmd() string  { return collector.CommandErrorEvents }
func (x *SpanEvents) Cmd() string   { return collector.CommandSpanEvents }
func (x *ErrorHeap) Cmd() string    { return collector.CommandErrors }
func (x *SlowSQLs) Cmd() string     { return collector.CommandSlowSQLs }
func (x *TxnTraces) Cmd() string    { return collector.CommandTraces }
func (x *TxnEvents) Cmd() string    { return collector.CommandTxnEvents }

func IntegrationData(p PayloadCreator, id AgentRunID, harvestStart time.Time) ([]byte, error) {
	audit, err := p.Audit(id, harvestStart)
	if nil != err {
		return nil, err
	}
	if nil != audit {
		return audit, nil
	}
	return p.Data(id, harvestStart)
}
