package collector

import (
	"crypto/x509"
	"io/ioutil"
	"path/filepath"
)

var (
	DefaultCertPool *x509.CertPool
)

func newCertPoolFromFiles(files []string) (*x509.CertPool, error) {
	pool := x509.NewCertPool()

	for _, f := range files {
		b, err := ioutil.ReadFile(f)
		if nil != err {
			return nil, err
		}
		pool.AppendCertsFromPEM(b)
	}

	return pool, nil
}

func NewCertPool(cafile, capath string) (*x509.CertPool, error) {
	var files []string
	var err error

	if "" != capath {
		files, err = filepath.Glob(filepath.Join(capath, "*.pem"))
		if nil != err {
			return nil, err
		}
	}

	if "" != cafile {
		files = append(files, cafile)
	}

	pool := x509.NewCertPool()

	for _, f := range files {
		b, err := ioutil.ReadFile(f)
		if nil != err {
			return nil, err
		}
		pool.AppendCertsFromPEM(b)
	}

	return pool, nil
}
