package newrelic

import (
	"testing"
	"time"

	"newrelic/utilization"
)

func TestConnectPayloadInternal(t *testing.T) {
	ramInitializer := new(uint64)
	*ramInitializer = 1000
	processors := 22
	util := &utilization.Data{
		MetadataVersion:   1,
		LogicalProcessors: &processors,
		RamMiB:            ramInitializer,
		Hostname:          "some_host",
	}
	info := &AppInfo{
		License:           "the_license",
		Appname:           "one;two",
		AgentLanguage:     "php",
		AgentVersion:      "0.1",
		HostDisplayName:   "my_awesome_host",
		Settings:          map[string]interface{}{"a": "1", "b": true},
		Environment:       JSONString(`[["b", 2]]`),
		HighSecurity:      false,
		Labels:            JSONString(`[{"label_type":"c","label_value":"d"}]`),
		RedirectCollector: "collector.newrelic.com",
	}

	expected := &RawConnectPayload{
		Pid:             123,
		Language:        "php",
		Version:         "0.1",
		Host:            "some_host",
		HostDisplayName: "my_awesome_host",
		Settings:        map[string]interface{}{"a": "1", "b": true},
		AppName:         []string{"one", "two"},
		HighSecurity:    false,
		Labels:          JSONString(`[{"label_type":"c","label_value":"d"}]`),
		Environment:     JSONString(`[["b",2]]`),
		Identifier:      "one;two",
		Util:            util,
	}

	pid := 123
	b := info.ConnectPayloadInternal(pid, util)

	// Compare the string integer and string portions of the structs
	// TestConnectEncodedJSON will do a full comparison after being encoded to bytes
	if b == nil {
		t.Errorf("the struct is set to nil")
	}
	if b.Pid != expected.Pid {
		t.Errorf("expected: %d\nactual: %d", expected.Pid, b.Pid)
	}
	if b.Language != expected.Language {
		t.Errorf("expected: %s\nactual: %s", expected.Language, b.Language)
	}
	if b.Version != expected.Version {
		t.Errorf("expected: %s\nactual: %s", expected.Version, b.Version)
	}
	if b.Host != expected.Host {
		t.Errorf("expected: %s\nactual: %s", expected.Host, b.Host)
	}
	if b.HostDisplayName != expected.HostDisplayName {
		t.Errorf("expected: %s\nactual: %s", expected.HostDisplayName, b.HostDisplayName)
	}
	if b.Identifier != expected.Identifier {
		t.Errorf("expected: %s\nactual: %s", expected.Identifier, b.Identifier)
	}
	if b.Util.MetadataVersion != expected.Util.MetadataVersion {
		t.Errorf("expected: %d\nactual: %d", expected.Util.MetadataVersion, b.Util.MetadataVersion)
	}
	if b.Util.Hostname != expected.Util.Hostname {
		t.Errorf("expected: %s\nactual: %s", expected.Util.Hostname, b.Util.Hostname)
	}
}

func TestConnectPayloadEncoded(t *testing.T) {
	ramInitializer := new(uint64)
	*ramInitializer = 1000
	processors := 22
	util := &utilization.Data{
		MetadataVersion:   1,
		LogicalProcessors: &processors,
		RamMiB:            ramInitializer,
		Hostname:          "some_host",
	}
	info := &AppInfo{
		License:           "the_license",
		Appname:           "one;two",
		AgentLanguage:     "php",
		AgentVersion:      "0.1",
		HostDisplayName:   "my_awesome_host",
		Settings:          map[string]interface{}{"a": "1", "b": true},
		Environment:       JSONString(`[["b", 2]]`),
		HighSecurity:      false,
		Labels:            JSONString(`[{"label_type":"c","label_value":"d"}]`),
		RedirectCollector: "collector.newrelic.com",
	}

	pid := 123
	expected := `[` +
		`{` +
		`"pid":123,` +
		`"language":"php",` +
		`"agent_version":"0.1",` +
		`"host":"some_host",` +
		`"display_host":"my_awesome_host",` +
		`"settings":{"a":"1","b":true},` +
		`"app_name":["one","two"],` +
		`"high_security":false,` +
		`"labels":[{"label_type":"c","label_value":"d"}],` +
		`"environment":[["b",2]],` +
		`"identifier":"one;two",` +
		`"utilization":{"metadata_version":1,"logical_processors":22,"total_ram_mib":1000,"hostname":"some_host"}` +
		`}` +
		`]`

	b, err := EncodePayload(info.ConnectPayloadInternal(pid, util))
	if err != nil {
		t.Error(err)
	} else if string(b) != expected {
		t.Errorf("expected: %s\nactual: %s", expected, string(b))
	}

	// an empty string for the HostDisplayName should not produce JSON
	info.HostDisplayName = ""
	expected = `[` +
		`{` +
		`"pid":123,` +
		`"language":"php",` +
		`"agent_version":"0.1",` +
		`"host":"some_host",` +
		`"settings":{"a":"1","b":true},` +
		`"app_name":["one","two"],` +
		`"high_security":false,` +
		`"labels":[{"label_type":"c","label_value":"d"}],` +
		`"environment":[["b",2]],` +
		`"identifier":"one;two",` +
		`"utilization":{"metadata_version":1,"logical_processors":22,"total_ram_mib":1000,"hostname":"some_host"}` +
		`}` +
		`]`

	b, err = EncodePayload(info.ConnectPayloadInternal(pid, util))
	if err != nil {
		t.Error(err)
	} else if string(b) != expected {
		t.Fatal(string(b))
	}
}

func TestNeedsConnectAttempt(t *testing.T) {
	var app App

	now := time.Date(2015, time.January, 10, 23, 0, 0, 0, time.UTC)

	app.state = AppStateUnknown
	app.lastConnectAttempt = now.Add(-AppConnectAttemptBackoff)
	if !app.NeedsConnectAttempt(now, AppConnectAttemptBackoff) {
		t.Fatal(now, app.lastConnectAttempt, app.state)
	}

	app.state = AppStateUnknown
	app.lastConnectAttempt = now
	if app.NeedsConnectAttempt(now, AppConnectAttemptBackoff) {
		t.Fatal(now, app.lastConnectAttempt, app.state)
	}

	app.state = AppStateConnected
	app.lastConnectAttempt = now.Add(-AppConnectAttemptBackoff)
	if app.NeedsConnectAttempt(now, AppConnectAttemptBackoff) {
		t.Fatal(now, app.lastConnectAttempt, app.state)
	}

	app.state = AppStateInvalidLicense
	app.lastConnectAttempt = now.Add(-AppConnectAttemptBackoff)
	if app.NeedsConnectAttempt(now, AppConnectAttemptBackoff) {
		t.Fatal(now, app.lastConnectAttempt, app.state)
	}

	app.state = AppStateDisconnected
	app.lastConnectAttempt = now.Add(-AppConnectAttemptBackoff)
	if app.NeedsConnectAttempt(now, AppConnectAttemptBackoff) {
		t.Fatal(now, app.lastConnectAttempt, app.state)
	}
}
