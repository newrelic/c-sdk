package main

import (
	"errors"
	"expvar"
	"fmt"
	"net"
	"net/http"
	_ "net/http/pprof" // enable profiling api
	"os"
	"os/signal"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"syscall"
	"time"

	"newrelic"
	"newrelic/config"
	"newrelic/log"
	"newrelic/version"
)

type workerError struct {
	Component string // component that failed, e.g. listener
	Respawn   bool   // restart process?
	Err       error  // underlying cause
}

func (e *workerError) Error() string {
	if e == nil {
		return "<nil>"
	}
	return e.Component + ": " + e.Err.Error()
}

// runWorker starts the listener and processor and does not return until a
// signal is received indicating the worker should shut down, or a fatal
// error occurs.
func runWorker(cfg *Config) {
	if cfg.MaxFiles > 0 {
		raiseFileLimit(cfg.MaxFiles)
	}

	if cfg.AuditFile != "" {
		if err := log.InitAudit(cfg.AuditFile); err != nil {
			log.Errorf("unable to open audit log: %v", err)
			setExitStatus(1)
			return
		}
	}

	if 0 != cfg.PProfPort {
		addr := net.JoinHostPort("127.0.0.1", strconv.Itoa(cfg.PProfPort))

		log.Infof("pprof enabled at %s", addr)

		// Uncomment this to enable the /debug/pprof/block endpoint.
		// This adds measurable overhead to the daemon, so only enable
		// it when necessary.
		// runtime.SetBlockProfileRate(1)

		// Publish version info.
		versionInfo := &struct{ Number, Build string }{version.Number, version.Commit}
		expvar.Publish("version", expvar.Func(func() interface{} { return versionInfo }))

		go func() {
			err := http.ListenAndServe(addr, nil)
			if err != nil {
				log.Debugf("pprof server error: %v", err)
			}
		}()
	}

	errorChan := make(chan error)
	signalChan := make(chan os.Signal, 1)
	signal.Notify(signalChan, syscall.SIGTERM)
	if cfg.Foreground {
		signal.Notify(signalChan, syscall.SIGINT)
	}

	clientCfg := &newrelic.ClientConfig{
		CAFile: cfg.CAFile,
		CAPath: cfg.CAPath,
		Proxy:  cfg.Proxy,
	}

	log.Infof("collector configuration is %+v", clientCfg)

	client, err := newrelic.NewClient(clientCfg)
	if nil != err {
		log.Errorf("unable to create client: %v", err)
		setExitStatus(1)
		return
	}

	if cfg.AppTimeout < 0 {
		cfg.AppTimeout = config.Timeout(newrelic.DefaultAppTimeout)
		log.Errorf("application inactivity timeout cannot be negative, using default of %v",
			cfg.AppTimeout)
	}

	p := newrelic.NewProcessor(newrelic.ProcessorConfig{
		Client:          client,
		IntegrationMode: cfg.IntegrationMode,
		UtilConfig:      cfg.MakeUtilConfig(),
		AppTimeout:      time.Duration(cfg.AppTimeout),
	})
	go processTxnData(errorChan, p)

	select {
	case <-listenAndServe(cfg.BindAddr, errorChan, p):
		log.Debugf("listener shutdown - exiting")
	case err := <-errorChan:
		if err != nil {
			log.Errorf("%v", err)
			if we, ok := err.(*workerError); ok && we.Respawn {
				setExitStatus(3)
			} else {
				setExitStatus(1)
			}
		}
	case caught := <-signalChan:
		log.Infof("worker received signal %d - exiting", caught)
	}
}

// listenAndServe starts and supervises the listener. If the listener
// terminates with an error, it is sent on errorChan; otherwise, the
// returned channel is closed to indicate a clean exit.
func listenAndServe(address string, errorChan chan<- error, p *newrelic.Processor) <-chan struct{} {
	doneChan := make(chan struct{})

	go func() {
		defer crashGuard("listener", errorChan)

		addr, err := parseBindAddr(address)
		if err != nil {
			errorChan <- &workerError{Component: "listener", Err: err}
			return
		}

		if addr.Network() == "unix" && !strings.HasPrefix(addr.String(), "@") {
			err := os.Remove(addr.String())
			if err != nil && !os.IsNotExist(err) {
				errorChan <- &workerError{
					Component: "listener",
					Err: fmt.Errorf("unable to remove stale sock file: %v"+
						" - another daemon may already be running?", err),
				}
				return
			}
		}

		err = newrelic.ListenAndServe(addr.Network(), addr.String(), newrelic.CommandsHandler{Processor: p})
		if err != nil {
			respawn := true

			// Some older RHEL 5.x linux kernels incorrectly handle missing system
			// calls (here: epoll_create1), which manifests as an EBADF error when
			// creating the listener socket.
			if runtime.GOOS == "linux" {
				perr, ok := err.(*net.OpError)
				if ok && perr.Err == syscall.EBADF {
					respawn = false
					err = borkedSyscallError("epoll_create1")
				}
			}

			errorChan <- &workerError{
				Component: "listener",
				Respawn:   respawn,
				Err:       err,
			}

			return
		}

		close(doneChan)
	}()

	return doneChan
}

// processTxnData starts and supervises the processor. We expect the
// processor to run for the lifetime of the process. Therefore, if the
// processor terminates, it is treated as a fatal error.
func processTxnData(errorChan chan<- error, p *newrelic.Processor) {
	defer crashGuard("processor", errorChan)

	err := p.Run()
	if err != nil {
		errorChan <- &workerError{
			Component: "processor",
			Respawn:   true,
			Err:       err,
		}
	}
}

func crashGuard(component string, errorChan chan<- error) {
	if err := recover(); err != nil {
		// Stacktraces captured during a panic are handled differently:  This
		// will contain the stack frame where the panic originated.
		stack := log.StackTrace()
		errorChan <- &workerError{
			Component: component,
			Respawn:   true,
			Err:       fmt.Errorf("panic %v\n%s", err, stack),
		}
	}
}

// parseBindAddr parses and validates the listener address.
func parseBindAddr(s string) (address net.Addr, err error) {
	const maxUnixLen = 106

	// '@' prefix specifies a Linux abstract domain socket.
	if runtime.GOOS == "linux" && strings.HasPrefix(s, "@") {
		if len(s) > maxUnixLen {
			return nil, fmt.Errorf("sock file length must be less than %d characters", maxUnixLen)
		}
		return &net.UnixAddr{Name: s, Net: "unix"}, nil
	}

	if strings.Contains(s, "/") {
		if !filepath.IsAbs(s) {
			return nil, errors.New("sock file must be an absolute path")
		} else if len(s) > maxUnixLen {
			return nil, fmt.Errorf("sock file length must be less than %d characters", maxUnixLen)
		}
		return &net.UnixAddr{Name: s, Net: "unix"}, nil
	}

	// For TCP, we only support binding to the loopback address.
	port, err := strconv.Atoi(s)
	if err != nil || port < 1 || port > 65534 {
		return nil, fmt.Errorf("invalid port %q - must be between 1 and 65534", s)
	}

	return &net.TCPAddr{IP: net.IPv4(127, 0, 0, 1), Port: port}, nil
}

// raiseFileLimit attempts to raise the soft limit for open file
// descriptors to be at least n. If the proposed minimum is larger than
// the hard limit an attempt will also be made to raise the hard limit.
// Raising the hard limit requires super-user privileges.
func raiseFileLimit(n uint64) {
	softLimit, hardLimit, err := getFileLimits()
	if err != nil {
		log.Warnf(`unable to increase file limit, the current value could`+
			` not be retrieved. If you are using an init script to start`+
			` the New Relic Daemon trying adding "ulimit -n %d" to your`+
			` init script. The error was %v.`, n, err)
		return
	}

	if n <= softLimit {
		return
	}

	// Maintain the following invariant: softLimit <= hardLimit
	// Failure to abide makes The Dude sad. Also causes EINVAL.

	if n > hardLimit {
		// The hard limit also needs to be raised. Try to raise the soft and
		// hard limits at the same time.
		err := setFileLimits(n, n)
		if err == nil {
			log.Infof("increased file limit to %d", n)
			return
		}

		// Couldn't raise the hard limit. Log the failure and fall through
		// below to raise the soft limit as high as we can.
		if err == syscall.EPERM {
			log.Warnf("unable to increase file hard limit from %d to %d."+
				" Raising the hard limit requires super-user privileges,"+
				" please contact your system administrator for assistance."+
				" An attempt will be made to raise the soft limit to %[1]d.",
				hardLimit, n)
		} else {
			log.Warnf("unable to increase file hard limit from %d to %d."+
				" The error was %v. An attempt will be made to raise the"+
				" soft limit to %[1]d.", hardLimit, n, err)
		}
	}

	softLimit = n

	// Ensure softLimit = min(n, hardLimit). We may have failed to raise
	// the hard limit to be greater than or equal to n above.
	if softLimit > hardLimit {
		softLimit = hardLimit
	}

	err = setFileLimits(softLimit, hardLimit)
	if err != nil {
		log.Warnf("unable to increase file limit: %v", err)
		return
	}

	log.Infof("increased file limit to %d", softLimit)
}
