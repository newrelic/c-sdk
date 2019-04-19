// +build use_system_certs

package collector

import (
	"crypto/x509"
	"newrelic/log"
)

func init() {
	pool, err := x509.SystemCertPool()
	if err != nil {
		log.Warnf("unable to load the system certificate pool; "+
			"communication with New Relic will likely be unsuccessful: %v", err)
		pool = x509.NewCertPool()
	}

	DefaultCertPool = pool
}
