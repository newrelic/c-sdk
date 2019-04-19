package newrelic

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"strings"
	"time"

	"newrelic/collector"
	"newrelic/utilization"
)

// AgentRunID is a string as of agent listener protocol version 14.
type AgentRunID string

func (id AgentRunID) String() string {
	return string(id)
}

type AppState int

const (
	AppStateUnknown AppState = iota
	AppStateConnected
	AppStateInvalidLicense
	AppStateDisconnected
	AppStateInvalidSecurityPolicies
)

// An AppKey uniquely identifies an application.
type AppKey struct {
	License           collector.LicenseKey
	Appname           string
	RedirectCollector string
	HighSecurity      bool
	AgentLanguage     string
	AgentPolicies     string
}

// AppInfo encapsulates information provided by an agent about an
// application. The information is used to construct part of the connect
// message sent to the collector, and the fields should not be modified.
type AppInfo struct {
	License                   collector.LicenseKey
	Appname                   string
	AgentLanguage             string
	AgentVersion              string
	HostDisplayName           string
	Settings                  map[string]interface{}
	Environment               JSONString
	HighSecurity              bool
	Labels                    JSONString
	RedirectCollector         string
	SecurityPolicyToken       string
	SupportedSecurityPolicies AgentPolicies
}

func (info *AppInfo) String() string {
	return info.Appname
}

// Structure of the security policies used on Preconnect and Connect
type SecurityPolicy struct {
	Enabled  bool `json:"enabled"`
	Required bool `json:"required,omitempty"`
}

type RawPreconnectPayload struct {
	SecurityPolicyToken string `json:"security_policies_token,omitempty"`
}

type RawConnectPayload struct {
	Pid              int                       `json:"pid"`
	Language         string                    `json:"language"`
	Version          string                    `json:"agent_version"`
	Host             string                    `json:"host"`
	HostDisplayName  string                    `json:"display_host,omitempty"`
	Settings         map[string]interface{}    `json:"settings"`
	AppName          []string                  `json:"app_name"`
	HighSecurity     bool                      `json:"high_security"`
	Labels           JSONString                `json:"labels"`
	Environment      JSONString                `json:"environment"`
	Identifier       string                    `json:"identifier"`
	Util             *utilization.Data         `json:"utilization,omitempty"`
	SecurityPolicies map[string]SecurityPolicy `json:"security_policies,omitempty"`
}

// PreconnectReply contains all of the fields from the app preconnect command reply
// that are used in the daemon.
type PreconnectReply struct {
	Collector        string                    `json:"redirect_host"`
	SecurityPolicies map[string]SecurityPolicy `json:"security_policies"`
}

// ConnectReply contains all of the fields from the app connect command reply
// that are used in the daemon.  The reply contains many more fields, but most
// of them are used in the agent.
type ConnectReply struct {
	ID                *AgentRunID            `json:"agent_run_id"`
	MetricRules       MetricRules            `json:"metric_name_rules"`
	DataMethods       *collector.DataMethods `json:"data_methods"`
	SamplingFrequency int                    `json:"sampling_target_period_in_seconds"`
	SamplingTarget    int                    `json:"sampling_target"`
}

// An App represents the state of an application.
type App struct {
	state               AppState
	collector           string
	lastConnectAttempt  time.Time
	connectTime         time.Time
	harvestFrequency    time.Duration
	samplingTarget      uint16
	info                *AppInfo
	connectReply        *ConnectReply
	RawSecurityPolicies []byte
	RawConnectReply     []byte
	HarvestTrigger      HarvestTriggerFunc
	LastActivity        time.Time
	Rules               MetricRules
}

func (app *App) String() string {
	return app.info.String()
}

func (info *AppInfo) Key() AppKey {
	return AppKey{
		License:           info.License,
		Appname:           info.Appname,
		RedirectCollector: info.RedirectCollector,
		HighSecurity:      info.HighSecurity,
		AgentLanguage:     info.AgentLanguage,
		AgentPolicies:     info.SupportedSecurityPolicies.getSupportedPoliciesHash(),
	}
}

func (app *App) Key() AppKey {
	return app.info.Key()
}

func NewApp(info *AppInfo) *App {
	now := time.Now()

	return &App{
		state:              AppStateUnknown,
		collector:          "",
		lastConnectAttempt: time.Time{},
		info:               info,
		HarvestTrigger:     nil,
		LastActivity:       now,
	}
}

func EncodePayload(payload interface{}) ([]byte, error) {
	buf := &bytes.Buffer{}
	buf.Grow(2048)
	buf.WriteByte('[')

	enc := json.NewEncoder(buf)
	if err := enc.Encode(&payload); err != nil {
		return nil, err
	}

	// json.Encoder always writes a trailing newline, replace it with the
	// closing bracket for the connect array.
	buf.Bytes()[buf.Len()-1] = ']'

	return buf.Bytes(), nil
}

func (info *AppInfo) ConnectPayloadInternal(pid int, util *utilization.Data) *RawConnectPayload {
	// Per spec, the hostname we send up in ConnectPayload MUST be the same as the
	// hostname we send up in Utilization.

	var hostname string
	if util != nil {
		hostname = util.Hostname
	}

	data := &RawConnectPayload{
		Pid:             pid,
		Language:        info.AgentLanguage,
		Version:         info.AgentVersion,
		Host:            hostname,
		HostDisplayName: stringLengthByteLimit(info.HostDisplayName, HostLengthByteLimit),
		Settings:        info.Settings,
		AppName:         strings.Split(info.Appname, ";"),
		HighSecurity:    info.HighSecurity,
		Environment:     info.Environment,
		// This identifier is used by the collector to look up the real agent. If an
		// identifier isn't provided, the collector will create its own based on the
		// first appname, which prevents a single daemon from connecting "a;b" and
		// "a;c" at the same time.
		//
		// Providing the identifier below works around this issue and allows users
		// more flexibility in using application rollups.
		Identifier: info.Appname,
		Util:       util,
	}

	if len(info.Labels) > 0 {
		data.Labels = info.Labels
	} else {
		data.Labels = JSONString("[]")
	}

	return data
}

// ConnectPayload creates the JSON of a connect request to be sent to the
// New Relic backend.
//
// Utilization is always expected to be present.
func (info *AppInfo) ConnectPayload(util *utilization.Data) *RawConnectPayload {
	return info.ConnectPayloadInternal(os.Getpid(), util)
}

func (info *AppInfo) initSettings(data []byte) {
	var dataDec interface{}

	err := json.Unmarshal(data, &dataDec)
	if err != nil {
		return
	}

	dataMap, ok := dataDec.(map[string]interface{})
	if ok {
		info.Settings = dataMap
	}
}

func (app *App) NeedsConnectAttempt(now time.Time, backoff time.Duration) bool {
	if app.state != AppStateUnknown {
		return false
	}
	if now.Sub(app.lastConnectAttempt) >= backoff {
		return true
	}
	return false
}

func parseConnectReply(rawConnectReply []byte) (*ConnectReply, error) {
	var c ConnectReply

	err := json.Unmarshal(rawConnectReply, &c)
	if nil != err {
		return nil, err
	}
	if nil == c.ID {
		return nil, errors.New("missing agent run id")
	}
	return &c, nil
}

// Inactive determines whether the elapsed time since app last had activity
// exceeds a threshold.
func (app *App) Inactive(threshold time.Duration) bool {
	if threshold < 0 {
		panic(fmt.Errorf("invalid inactivity threshold: %v", threshold))
	}
	return time.Since(app.LastActivity) > threshold
}
