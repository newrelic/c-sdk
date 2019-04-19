// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package proxy

import (
	"bytes"
	"errors"
	"io"
	"io/ioutil"
	"net"
	"testing"
	"time"
)

func checkRequest(t *testing.T, req []byte, wantPort int, wantIP net.IP) {
	if req[0] != socks4Version {
		t.Errorf("wrong version: got %d, want %d", req[0], socks4Version)
	}

	if req[1] != socks4Connect {
		t.Errorf("wrong command: got %d, want %d", req[1], socks4Connect)
	}

	gotPort := int(req[2])<<8 | int(req[3])
	if gotPort != wantPort {
		t.Errorf("wrong port: got %d, want %d", gotPort, wantPort)
	}

	gotIP := net.IPv4(req[4], req[5], req[6], req[7])
	if !gotIP.Equal(wantIP) {
		t.Errorf("wrong IP: got %v, want %v", gotIP, wantIP)
	}
}

func TestNoAuth(t *testing.T) {
	proxy := &socks4{network: "tcp", addr: "127.0.0.1"}
	req, err := proxy.buildRequest("127.0.0.1:1080")
	if err != nil {
		t.Fatalf("buildRequest failed: %v", err)
	}

	if len(req) != 9 {
		t.Errorf("len(req) != 9, req=%#v", req)
	}

	checkRequest(t, req, 1080, net.IPv4(127, 0, 0, 1))

	if req[8] != 0 {
		t.Errorf("request does not have trailing NUL, got %#02x", req[8])
	}
}

func TestAuth(t *testing.T) {
	proxy := &socks4{user: "john doe", network: "tcp", addr: "127.0.0.1"}
	req, err := proxy.buildRequest("127.0.0.1:1080")
	if err != nil {
		t.Fatalf("buildRequest failed: %v", err)
	}

	if len(req) != 17 {
		t.Errorf("len(req) != 17, req=%#v", req)
	}

	checkRequest(t, req, 1080, net.IPv4(127, 0, 0, 1))

	if string(req[8:16]) != "john doe" {
		t.Errorf("wrong username: got %#q want %#q", req[8:16], "john doe")
	}

	if req[16] != 0 {
		t.Errorf("request does not have trailing NUL, got %#02x", req[16])
	}
}

func TestLookup(t *testing.T) {
	proxy := &socks4{network: "tcp", addr: "127.0.0.1"}
	req, err := proxy.buildRequest("localhost:1080")
	if err != nil {
		t.Fatalf("buildRequest failed: %v", err)
	}

	if len(req) != 9 {
		t.Errorf("len(req) != 9, req=%#v", req)
	}

	checkRequest(t, req, 1080, net.IPv4(127, 0, 0, 1))

	if req[8] != 0 {
		t.Errorf(`username is not empty: got %#q, want "\x00"`, req[8:])
	}
}

func TestDisableLookup(t *testing.T) {
	proxy := &socks4{network: "tcp", addr: "127.0.0.1:1080", disableLookup: true}
	req, err := proxy.buildRequest("localhost:1080")
	if err != nil {
		t.Fatalf("buildRequest failed: %v", err)
	}

	if len(req) != 19 {
		t.Errorf("len(req) != 19, req=%#v", req)
	}

	checkRequest(t, req, 1080, net.IPv4(0, 0, 0, 1))

	if req[8] != 0 {
		t.Errorf(`username is not empty: got %#q, want "\x00"`, req[8:])
	}

	if string(req[9:18]) != "localhost" {
		t.Errorf("wrong hostname: got %#q want %#q", req[9:18], "localhost")
	}

	if req[18] != 0 {
		t.Errorf("hostname does not have trailing NUL, got %#02x", req[16])
	}
}

type fakeConn struct {
	io.Reader
	io.Writer
	closed bool
}

func (p *fakeConn) Close() error {
	p.closed = true
	return nil
}

func (*fakeConn) LocalAddr() net.Addr {
	return nil
}

func (*fakeConn) RemoteAddr() net.Addr {
	return nil
}

func (*fakeConn) SetDeadline(t time.Time) error {
	return errors.New("not implemented")
}

func (*fakeConn) SetReadDeadline(t time.Time) error {
	return errors.New("not implemented")
}

func (*fakeConn) SetWriteDeadline(t time.Time) error {
	return errors.New("not implemented")
}

func TestRoundTrip(t *testing.T) {
	proxy := &socks4{network: "tcp", addr: "127.0.0.1:1080", disableLookup: true}
	req, err := proxy.buildRequest("localhost:8000")
	if err != nil {
		t.Fatal("buildRequest failed:", err)
	}

	// request rejected
	conn := &fakeConn{
		Reader: bytes.NewReader([]byte{0, 0x5b, 0, 0, 0, 0, 0, 0}),
		Writer: ioutil.Discard,
	}
	err = proxy.roundtrip(conn, req)
	if err == nil {
		t.Error("failed to handle rejected error")
	}

	// identd unreachable
	conn = &fakeConn{
		Reader: bytes.NewReader([]byte{0, 0x5c, 0, 0, 0, 0, 0, 0}),
		Writer: ioutil.Discard,
	}
	err = proxy.roundtrip(conn, req)
	if err == nil {
		t.Error("failed to handle identd unreachable")
	}

	// uid mismatch
	conn = &fakeConn{
		Reader: bytes.NewReader([]byte{0, 0x5d, 0, 0, 0, 0, 0, 0}),
		Writer: ioutil.Discard,
	}
	err = proxy.roundtrip(conn, req)
	if err == nil {
		t.Error("failed to handle uid mismatch")
	}

	// unknown error
	conn = &fakeConn{
		Reader: bytes.NewReader([]byte{0, 0xff, 0, 0, 0, 0, 0, 0}),
		Writer: ioutil.Discard,
	}
	err = proxy.roundtrip(conn, req)
	if err == nil {
		t.Error("failed to handle unknown error")
	}
}
