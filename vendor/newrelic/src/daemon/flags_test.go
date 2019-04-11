package main

import (
	"flag"
	"reflect"
	"testing"

	"newrelic/config"
)

func TestParseFlagsWithAll(t *testing.T) {
	cfg := &Config{}

	cfgExpected := &Config{
		BindAddr:        "30",
		Proxy:           "collector.newrelic.com",
		Pidfile:         "pid.file",
		NoPidfile:       true,
		LogFile:         "log.log",
		LogLevel:        3,
		AuditFile:       "audit.log",
		ConfigFile:      "file.config",
		TLS:             true,
		Foreground:      true,
		Role:            0,
		Agent:           true,
		PProfPort:       1,
		CAPath:          "ca.path",
		CAFile:          "ca.file",
		IntegrationMode: true,
	}

	args := []string{
		"-c", "file.config",
		"-port", "30",
		"-proxy", "collector.newrelic.com",
		"-pidfile", "pid.file",
		"-no-pidfile",
		"-logfile", "log.log",
		"-loglevel", "info",
		"-auditlog", "audit.log",
		"-tls",
		"-f",
		"-foreground",
		"-agent",
		"-cafile", "ca.file",
		"-capath", "ca.path",
		"-integration",
		"-pprof", "1",
	}

	flagSet := createFlagSet(cfg)
	flagSet.Parse(args)

	if !reflect.DeepEqual(cfg, cfgExpected) {
		t.Errorf("Actual: %#v \nExpected: %#v", cfg, cfgExpected)
	}
}

func TestParseFlagsWithNone(t *testing.T) {
	cfg := &Config{}

	cfgExpected := &Config{}

	args := []string{""}
	flagSet := createFlagSet(cfg)
	flagSet.Parse(args)

	if !reflect.DeepEqual(cfg, cfgExpected) {
		t.Errorf("Actual: %#v \nExpected: %#v", cfg, cfgExpected)
	}
}

func TestParseFlagsWithBad(t *testing.T) {
	cfg := &Config{}

	cfgExpected := &Config{}

	args := []string{
		"-q",
		"-cafile",
	}
	flagSet := createFlagSet(cfg)
	flagSet.Parse(args)

	if !reflect.DeepEqual(cfg, cfgExpected) {
		t.Errorf("Actual: %#v \nExpected: %#v", cfg, cfgExpected)
	}
}

// Config file attributes are appropriately parsed and modified within Config
func TestParseConfigFile(t *testing.T) {

	cfg := &Config{
		ConfigFile: "../newrelic/sample_config/config1.cfg",
		LogLevel:   0,
		LogFile:    "",
	}

	cfgExpected := &Config{
		ConfigFile: "../newrelic/sample_config/config1.cfg",
		LogLevel:   4,
		LogFile:    "/var/log/newrelic/newrelic-daemon.log",
	}

	if err := parseConfigFile(cfg); err != nil {
		t.Errorf("invalid configuration: %v\n", err)
	}

	if !reflect.DeepEqual(cfg, cfgExpected) {
		t.Errorf("Actual: %#v \nExpected: %#v", cfg, cfgExpected)
	}
}

func TestParseCommandLineTakesPrecedence(t *testing.T) {
	cfg := &Config{
		ConfigFile: "../newrelic/sample_config/config1.cfg",
	}

	cfgExpected := &Config{
		ConfigFile: "../newrelic/sample_config/config1.cfg",
		LogFile:    "foo.log",
		LogLevel:   1,
	}

	args := []string{
		"-loglevel", "error",
		"-logfile", "foo.log",
	}

	flagSet := createFlagSet(cfg)
	flagSet.Parse(args)

	if err := parseConfigFile(cfg); err != nil {
		t.Errorf("invalid configuration: %v\n", err)
	}

	// parse again to ensure command line arguments take precendence
	flagSet.Parse(args)

	if !reflect.DeepEqual(cfg, cfgExpected) {
		t.Errorf("Actual: %#v \nExpected: %#v", cfg, cfgExpected)
	}
}

func TestDefineShim(t *testing.T) {
	var err error
	var cfg struct {
		S string
		I int
	}

	fs := flag.NewFlagSet("TestVar", flag.ContinueOnError)
	fs.Var(config.NewFlagParserShim(&cfg), "d", "")

	err = fs.Parse([]string{"-d", "S = abc", "-d", "I=42"})
	if err == nil {
		if cfg.S != "abc" {
			t.Errorf("configValue.Set(%q) = %q, want %q", "S = abc", cfg.S, "abc")
		}

		if cfg.I != 42 {
			t.Errorf("configValue.Set(%q) = %d, want %d", "I=42", cfg.I, 42)
		}
	} else {
		t.Errorf("configValue.Set(%q) = %v", "-d 'S = abc' -d 'I=42'", err)
	}

	cfg.S = ""
	cfg.I = 0

	err = fs.Parse([]string{"-d", "I = abc"})
	if err == nil {
		t.Errorf("ParseString(%q) = nil, want error", "I = abc")
	}
}

func TestDefineShimHighlevel(t *testing.T) {
	cfg := &Config{}

	// basic test
	args := []string{"--define", "pidfile=/distinctively/insi.pid"}
	flagSet := createFlagSet(cfg)
	err := flagSet.Parse(args)
	if err != nil {
		t.Errorf("Error parsing flags: %s", err)
	}

	if cfg.Pidfile != "/distinctively/insi.pid" {
		t.Error("Failed to set pidfile with --define")
	}

	// test with multiple
	cfg = &Config{}
	args = []string{
		"--define", "pidfile =  fizz.pid",
		"--foreground", // why not
		"-define", "port= 1123",
	}
	flagSet = createFlagSet(cfg)
	err = flagSet.Parse(args)
	if err != nil {
		t.Errorf("Error parsing flags: %s", err)
	}

	if cfg.Pidfile != "fizz.pid" {
		t.Error("Failed to set pidfile with --define")
	}
	if cfg.BindAddr != "1123" {
		t.Error("Failed to set port with -define")
	}
	if !cfg.Foreground {
		t.Error("Parsing the foreground flag failed while testing defines. This is strange.")
	}

	// It looks like the second of any specified option or define is going to
	// take precedence, which I'm just going to calcify here as expected behavior.
	cfg = &Config{}
	args = []string{
		"--logfile", "/whoo/far/zazz",
		"--define", "logfile=/foo/bar/baz",
	}
	flagSet = createFlagSet(cfg)
	err = flagSet.Parse(args)
	if err != nil {
		t.Errorf("Error parsing flags: %s", err)
	}

	if cfg.LogFile != "/foo/bar/baz" {
		t.Error("Earlier pidfile overrides later define of pidfile")
	}

	// Let's test that the flag splits on the first equals sign as well.
	cfg = &Config{}
	args = []string{
		`--define=logfile=/better/than/good/its.log`,
		"--define", "ssl_ca_path=/blammo",
		`--define=ssl='false'`,
		`--define=proxy="doctorevil:correcthorsebatterystaple@proxy-server.bad.domain"`,
	}
	flagSet = createFlagSet(cfg)
	err = flagSet.Parse(args)
	if err != nil {
		t.Errorf("Error parsing flags: %s", err)
	}

	if cfg.LogFile != "/better/than/good/its.log" {
		t.Error("Setting option with --define=\"x=y\" failed.")
	}
	if cfg.CAPath != "/blammo" {
		t.Error("Setting option with --define=\"x=y\" failed.")
	}
	if cfg.Proxy != `doctorevil:correcthorsebatterystaple@proxy-server.bad.domain` {
		t.Errorf("Parsing a flag containing a quoted string gave an unexpected result. Actual: %s", cfg.Proxy)
	}

	// We can't use quotes to refer to the whole clause define refers to, though.
	// Looks like only the config parser accepts them.
	cfg = &Config{}
	args = []string{
		`--define="ssl=false"`,
	}
	flagSet = createFlagSet(cfg)
	err = flagSet.Parse(args)
	if err == nil {
		t.Error(`We can parse --define="a=b" now! Yippee!`)
	}
}
