// +build !linux

package sysinfo

import "os"

// Hostname returns the host name reported by the kernel.
func Hostname() (string, error) {
	return os.Hostname()
}
