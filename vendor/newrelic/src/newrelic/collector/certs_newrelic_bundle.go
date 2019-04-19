// +build !use_system_certs

package collector

import "crypto/x509"

func init() {
	pool := x509.NewCertPool()
	pool.AppendCertsFromPEM([]byte(nrCABundle))
	DefaultCertPool = pool
}
