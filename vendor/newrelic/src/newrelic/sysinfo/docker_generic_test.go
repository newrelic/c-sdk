// +build !linux

package sysinfo

import (
	"testing"
)

func TestDockerID(t *testing.T) {
	_, err := DockerID()
	if err != ErrFeatureUnsupported {
		t.Fatalf("Expected error %v; got: %v", ErrFeatureUnsupported, err)
	}
}
