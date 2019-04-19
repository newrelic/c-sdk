// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package proxy

import (
	"errors"
	"io"
	"net"
	"strconv"
)

// SOCKS4 returns a Dialer that makes SOCKSv4 connections to the given address.
// See: http://www.openssh.com/txt/socks4.protocol
func SOCKS4(network, addr string, auth *Auth, forward Dialer) (Dialer, error) {
	s := &socks4{
		network: network,
		addr:    addr,
		forward: forward,
	}
	if auth != nil {
		s.user = auth.User
	}
	return s, nil
}

// SOCKS4a returns a Dialer that makes SOCKSv4a connections to the given address.
// See: http://www.openssh.com/txt/socks4a.protocol
func SOCKS4a(network, addr string, auth *Auth, forward Dialer) (Dialer, error) {
	s := &socks4{
		network:       network,
		addr:          addr,
		forward:       forward,
		disableLookup: true,
	}
	if auth != nil {
		s.user = auth.User
	}
	return s, nil
}

type socks4 struct {
	user          string
	network, addr string
	forward       Dialer
	disableLookup bool // send host to proxy, SOCKS4a only
}

const (
	socks4Version = 4
	socks4Connect = 1
)

// socks4IsSentinel returns true if x is an invalid IP address indicating
// that the proxy connect request contains the destination hostname.
func socks4IsSentinel(x net.IP) bool {
	ip4 := x.To4()
	if ip4 == nil {
		return false
	}
	return ip4[0] == 0 && ip4[1] == 0 && ip4[2] == 0 && ip4[3] != 0
}

func (s *socks4) buildRequest(addr string) ([]byte, error) {
	host, portStr, err := net.SplitHostPort(addr)
	if err != nil {
		return nil, err
	}

	port, err := strconv.Atoi(portStr)
	if err != nil {
		return nil, errors.New("proxy: failed to parse port number: " + portStr)
	}
	if port < 1 || port > 0xffff {
		return nil, errors.New("proxy: port number out of range: " + portStr)
	}

	destIP := net.ParseIP(host)
	if destIP == nil && s.disableLookup {
		// An invalid IP address indicates that the connect request contains
		// the hostname.
		destIP = net.IPv4(0, 0, 0, 1)
	}

	if destIP == nil {
		addrs, err := net.LookupIP(host)
		if err != nil {
			return nil, err
		}

		// Use the first IPv4 address.
		for i := range addrs {
			if addrs[i].To4() != nil {
				destIP = addrs[i]
			}
		}
	}

	destIP = destIP.To4()
	if destIP == nil {
		return nil, errors.New("proxy: SOCKS4 does not support IPv6")
	}

	req := make([]byte, 0, 8+len(s.user)+1)
	req = append(req, socks4Version, socks4Connect)
	req = append(req, byte(port>>8), byte(port))
	req = append(req, destIP...)
	req = append(req, s.user...)
	req = append(req, 0)
	if socks4IsSentinel(destIP) {
		req = append(req, host...)
		req = append(req, 0)
	}
	return req, nil
}

func (s *socks4) roundtrip(c net.Conn, req []byte) error {
	if _, err := c.Write(req); err != nil {
		return &Error{"SOCKS4", nil, err}
	}

	var reply [8]byte
	if _, err := io.ReadFull(c, reply[:]); err != nil {
		return &Error{"SOCKS4", nil, err}
	}

	switch status := int(reply[1]); status {
	case 0x5a:
		// request granted
	case 0x5b:
		return &Error{"SOCKS4", c.RemoteAddr(), errors.New("failed to connect: request rejected")}
	case 0x5c:
		return &Error{"SOCKS4", c.RemoteAddr(), errors.New("failed to connect: identd is unreachable")}
	case 0x5d:
		return &Error{"SOCKS4", c.RemoteAddr(), errors.New("failed to connect: uid is mismatched")}
	default:
		return &Error{"SOCKS4", c.RemoteAddr(), errors.New("failed to connect: unknown error (" + strconv.Itoa(status) + ")")}
	}
	return nil
}

// Dial connects to the address addr on the network net via the SOCKS4 proxy.
func (s *socks4) Dial(network, addr string) (net.Conn, error) {
	switch network {
	case "tcp", "tcp4":
	default:
		return nil, errors.New("proxy: no support for SOCKS4 proxy connections of type " + network)
	}

	req, err := s.buildRequest(addr)
	if err != nil {
		return nil, err
	}

	conn, err := s.forward.Dial(s.network, s.addr)
	if err != nil {
		return nil, &Error{"SOCKS4", nil, err}
	}

	if err := s.roundtrip(conn, req); err != nil {
		conn.Close()
		return nil, err
	}
	return conn, nil
}
