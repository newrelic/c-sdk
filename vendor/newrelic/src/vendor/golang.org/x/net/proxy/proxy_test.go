// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package proxy

import (
	"bytes"
	"io"
	"net"
	"net/url"
	"strconv"
	"sync"
	"testing"
)

func TestFromURL(t *testing.T) {
	endSystem, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer endSystem.Close()
	gateway, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer gateway.Close()

	var wg sync.WaitGroup
	wg.Add(1)
	go socks5Gateway(t, gateway, endSystem, socks5Domain, &wg)

	url, err := url.Parse("socks5://user:password@" + gateway.Addr().String())
	if err != nil {
		t.Fatalf("url.Parse failed: %v", err)
	}
	proxy, err := FromURL(url, Direct)
	if err != nil {
		t.Fatalf("FromURL failed: %v", err)
	}
	_, port, err := net.SplitHostPort(endSystem.Addr().String())
	if err != nil {
		t.Fatalf("net.SplitHostPort failed: %v", err)
	}
	if c, err := proxy.Dial("tcp", "localhost:"+port); err != nil {
		t.Fatalf("FromURL.Dial failed: %v", err)
	} else {
		c.Close()
	}

	wg.Wait()
}

func TestSOCKS5(t *testing.T) {
	endSystem, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer endSystem.Close()
	gateway, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer gateway.Close()

	var wg sync.WaitGroup
	wg.Add(1)
	go socks5Gateway(t, gateway, endSystem, socks5IP4, &wg)

	proxy, err := SOCKS5("tcp", gateway.Addr().String(), nil, Direct)
	if err != nil {
		t.Fatalf("SOCKS5 failed: %v", err)
	}
	if c, err := proxy.Dial("tcp", endSystem.Addr().String()); err != nil {
		t.Fatalf("SOCKS5.Dial failed: %v", err)
	} else {
		c.Close()
	}

	wg.Wait()
}

func socks5Gateway(t *testing.T, gateway, endSystem net.Listener, typ byte, wg *sync.WaitGroup) {
	defer wg.Done()

	c, err := gateway.Accept()
	if err != nil {
		t.Errorf("net.Listener.Accept failed: %v", err)
		return
	}
	defer c.Close()

	b := make([]byte, 32)
	var n int
	if typ == socks5Domain {
		n = 4
	} else {
		n = 3
	}
	if _, err := io.ReadFull(c, b[:n]); err != nil {
		t.Errorf("io.ReadFull failed: %v", err)
		return
	}
	if _, err := c.Write([]byte{socks5Version, socks5AuthNone}); err != nil {
		t.Errorf("net.Conn.Write failed: %v", err)
		return
	}
	if typ == socks5Domain {
		n = 16
	} else {
		n = 10
	}
	if _, err := io.ReadFull(c, b[:n]); err != nil {
		t.Errorf("io.ReadFull failed: %v", err)
		return
	}
	if b[0] != socks5Version || b[1] != socks5Connect || b[2] != 0x00 || b[3] != typ {
		t.Errorf("got an unexpected packet: %#02x %#02x %#02x %#02x", b[0], b[1], b[2], b[3])
		return
	}
	if typ == socks5Domain {
		copy(b[:5], []byte{socks5Version, 0x00, 0x00, socks5Domain, 9})
		b = append(b, []byte("localhost")...)
	} else {
		copy(b[:4], []byte{socks5Version, 0x00, 0x00, socks5IP4})
	}
	host, port, err := net.SplitHostPort(endSystem.Addr().String())
	if err != nil {
		t.Errorf("net.SplitHostPort failed: %v", err)
		return
	}
	b = append(b, []byte(net.ParseIP(host).To4())...)
	p, err := strconv.Atoi(port)
	if err != nil {
		t.Errorf("strconv.Atoi failed: %v", err)
		return
	}
	b = append(b, []byte{byte(p >> 8), byte(p)}...)
	if _, err := c.Write(b); err != nil {
		t.Errorf("net.Conn.Write failed: %v", err)
		return
	}
}

func TestSOCKS4(t *testing.T) {
	endSystem, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer endSystem.Close()

	gateway, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer gateway.Close()

	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		defer wg.Done()

		c, err := gateway.Accept()
		if err != nil {
			t.Errorf("net.Listener.Accept failed: %v", err)
			return
		}
		defer c.Close()

		addr := endSystem.Addr().(*net.TCPAddr)
		checkSOCKS4(t, c, addr.Port, addr.IP, "user", "")

		reply := []byte{0, 0x5a, 0, 0, 0, 0, 0, 0} // request accepted
		if _, err := c.Write(reply); err != nil {
			t.Errorf("net.Conn.Write failed: %v", err)
			return
		}
	}()

	proxy, err := SOCKS4("tcp", gateway.Addr().String(), &Auth{User: "user"}, Direct)
	if err != nil {
		t.Fatalf("SOCKS4 failed: %v", err)
	}

	if c, err := proxy.Dial("tcp", endSystem.Addr().String()); err != nil {
		t.Fatalf("SOCKS4.Dial failed: %v", err)
	} else {
		c.Close()
	}

	wg.Wait()
}

func TestSOCKS4a(t *testing.T) {
	endSystem, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer endSystem.Close()

	gateway, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("net.Listen failed: %v", err)
	}
	defer gateway.Close()

	wantPort := endSystem.Addr().(*net.TCPAddr).Port
	wantIP := net.IPv4(0, 0, 0, 1)
	wantUser := "user"
	wantHost := "localhost"

	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		defer wg.Done()

		c, err := gateway.Accept()
		if err != nil {
			t.Errorf("net.Listener.Accept failed: %v", err)
			return
		}
		defer c.Close()

		checkSOCKS4(t, c, wantPort, wantIP, wantUser, wantHost)

		reply := []byte{0, 0x5a, 0, 0, 0, 0, 0, 0} // request accepted
		if _, err := c.Write(reply); err != nil {
			t.Errorf("net.Conn.Write failed: %v", err)
			return
		}
	}()

	proxy, err := SOCKS4a("tcp", gateway.Addr().String(), &Auth{User: "user"}, Direct)
	if err != nil {
		t.Fatalf("SOCKS4a failed: %v", err)
	}

	// Force hostname to be sent to gateway.
	host := net.JoinHostPort("localhost", strconv.Itoa(wantPort))
	if c, err := proxy.Dial("tcp", host); err != nil {
		t.Fatalf("SOCKS4a.Dial failed: %v", err)
	} else {
		c.Close()
	}

	wg.Wait()
}

func checkSOCKS4(t *testing.T, r io.Reader, wantPort int, wantIP net.IP, wantUser, wantHost string) {
	wantLen := 8 + len(wantUser) + 1
	if socks4IsSentinel(wantIP) {
		wantLen += len(wantHost) + 1
	}

	req := make([]byte, wantLen)
	if n, err := r.Read(req); err != nil {
		t.Error("io.Reader.Read failed:", err)
		return
	} else if n != wantLen {
		t.Error("got an incomplete packet:", req[:n])
		return
	}

	if req[0] != socks4Version || req[1] != socks4Connect {
		t.Errorf("got an unexpected packet: %#02x %#02x", req[0], req[1])
	}

	gotPort := int(req[2])<<8 | int(req[3])
	if gotPort != wantPort {
		t.Errorf("port = %d want %d", gotPort, wantPort)
	}

	gotIP := net.IPv4(req[4], req[5], req[6], req[7])
	if !gotIP.Equal(wantIP) {
		t.Errorf("wrong IP: got %v want %v", gotIP, wantIP)
	}

	req = req[8:]
	userEnd := bytes.IndexByte(req, 0)
	if userEnd == -1 {
		t.Errorf("wrong user: got %#q want %#q", req[8:], wantUser)
		return
	}

	if string(req[:userEnd]) != wantUser {
		t.Errorf("wrong user: got %#q want %#q", req[:userEnd], wantUser)
	}

	if socks4IsSentinel(wantIP) {
		gotHost := string(req[userEnd+1:])
		if gotHost[len(gotHost)-1] == 0 {
			gotHost = gotHost[:len(gotHost)-1]
		}
		if gotHost != wantHost {
			t.Errorf("wrong host: got %#q want %#q", gotHost, wantHost)
		}
	}
}
