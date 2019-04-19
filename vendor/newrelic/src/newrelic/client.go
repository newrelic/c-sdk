package newrelic

import "newrelic/collector"

type ClientConfig struct {
	CAFile string
	CAPath string
	Proxy  string
}

type Client collector.Client

// NewClient wraps collector.NewClient in order to ensure that the constants
// MaxOutboundConns and HarvestTimeout are used.  This wrapper allows for these
// constants to be kept in this package alongside the other limits.
func NewClient(cfg *ClientConfig) (Client, error) {
	realCfg := &collector.ClientConfig{
		CAFile:      cfg.CAFile,
		CAPath:      cfg.CAPath,
		Proxy:       cfg.Proxy,
		MaxParallel: MaxOutboundConns,
		Timeout:     HarvestTimeout,
	}
	return collector.NewClient(realCfg)
}
