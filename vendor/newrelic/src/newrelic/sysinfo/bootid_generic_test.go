// +build !linux

package sysinfo

import (
	"testing"
)

func TestBootID(t *testing.T) {
	_, err := BootID()
	if err != ErrFeatureUnsupported {
		t.Fatalf("Expected error %v; got: %v", ErrFeatureUnsupported, err)
	}
}
