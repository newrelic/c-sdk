#
# "What's in the bag? A shark or something?"
#
# No, Nicolas. It's a C SDK, and we're going to build it.
#
# Useful top level targets:
#
# - all:       Builds libnewrelic.a and newrelic-daemon
# - clean:     Removes all build products
# - daemon:    Builds newrelic-daemon
# - dynamic:   Builds libnewrelic.so
# - static:    Builds libnewrelic.a
# - run_tests: Runs the unit tests
# - tests:     Builds, but does not run, the unit tests
# - valgrind:  Runs the unit tests under valgrind
#

include make/config.mk

#
# Set up the appropriate flags for cmocka, since we use that for unit tests.
#
CMOCKA_LIB = $(C_AGENT_ROOT)/vendor/cmocka/build/src/libcmocka.a
CMOCKA_INCLUDE = -I$(C_AGENT_ROOT)/vendor/cmocka/include

export CMOCKA_LIB
export CMOCKA_INCLUDE

# OS X 10.11 does not provide pcre-config by default.
# Check whether it exists, and if not assume a sensible default.
PCRE_CFLAGS := $(shell pcre-config --cflags)
PCRE_LIBS := $(shell pcre-config --libs)

#
# We pull in the current SDK version from the VERSION file, and expose it to
# the source code as the NEWRELIC_VERSION preprocessor define.
#
AGENT_VERSION := $(shell if test -f VERSION; then cat VERSION; fi)
VERSION_FLAGS += -DNEWRELIC_VERSION=$(AGENT_VERSION)

export AGENT_VERSION VERSION_FLAGS

all: libnewrelic.a newrelic-daemon

ifeq (Darwin,$(UNAME))
#
# This rule builds a static axiom library and a static C SDK library, and
# then uses macOS's special libtool to smoosh them together into a single,
# beautiful library.
#
LIBTOOL := /usr/bin/libtool

libnewrelic.a: axiom src-static
	$(LIBTOOL) -static -o $@ vendor/newrelic/axiom/libaxiom.a src/libnewrelic.a
else
#
# This rule builds a static axiom library and a static C SDK library, and
# then uses GNU ar's MRI support to smoosh them together into a single,
# beautiful library.
libnewrelic.a: make/combine.mri axiom src-static
	$(AR) -M < $<
endif

.PHONY: static
static: libnewrelic.a

# Unlike axiom, we can't use a target-specific variable to send the CFLAGS and
# LDFLAGS, as those propagate to dependent rules and cmocka won't build with
# all the warnings we enabled. Instead, we'll use command line variables when
# invoking the sub-make to pass them in.
.PHONY: run_tests
run_tests: vendor libnewrelic.a
	$(MAKE) -C tests run_tests CFLAGS="$(C_AGENT_CFLAGS) -Wno-bad-function-cast" LDFLAGS="$(C_AGENT_LDFLAGS)"

.PHONY: tests
tests: vendor libnewrelic.a
	$(MAKE) -C tests tests CFLAGS="$(C_AGENT_CFLAGS) -Wno-bad-function-cast" LDFLAGS="$(C_AGENT_LDFLAGS)"

.PHONY: vendor
vendor:
	$(MAKE) -C vendor

.PHONY: axiom vendor/newrelic/axiom/libaxiom.a
axiom: vendor/newrelic/axiom/libaxiom.a

vendor/newrelic/axiom/libaxiom.a: export CFLAGS := $(C_AGENT_CFLAGS) -DNR_CAGENT
vendor/newrelic/axiom/libaxiom.a:
	$(MAKE) -C vendor/newrelic/axiom

.PHONY: axiom-clean
axiom-clean:
	$(MAKE) -C vendor/newrelic/axiom clean

daemon: newrelic-daemon

newrelic-daemon:
	$(MAKE) USE_SYSTEM_CERTS=1 -C vendor/newrelic daemon
	cp vendor/newrelic/bin/daemon newrelic-daemon

.PHONY: daemon-clean
daemon-clean:
	$(MAKE) -C vendor/newrelic daemon-clean
	rm -f newrelic-daemon

.PHONY: dynamic
dynamic: libnewrelic.so

.PHONY: valgrind
valgrind: vendor libnewrelic.a
	$(MAKE) -C tests valgrind

#
# We build the shared library at the top level, since it's easiest to just take
# the static library and have gcc wrap it in the appropriate shared library
# goop.
libnewrelic.so: libnewrelic.a
	$(CC) -shared -pthread -o $@ -Wl,--whole-archive $^ -Wl,--no-whole-archive $(PCRE_LIBS) -ldl

.PHONY: src-static
src-static:
	$(MAKE) -C src static

.PHONY: src-clean
src-clean:
	$(MAKE) -C src clean

.PHONY: tests-clean
tests-clean:
	$(MAKE) -C tests clean
	$(MAKE) -C vendor clean

.PHONY: clean
clean: axiom-clean daemon-clean src-clean tests-clean
	rm -f *.o libnewrelic.a libnewrelic.so

.PHONY: integration
integration: libnewrelic.so daemon
	./vendor/newrelic/bin/integration_runner --cgi=sh tests/integration

# Implicit rule for top level test programs.
%.o: %.c
	$(CC) $(C_AGENT_CPPFLAGS) $(VERSION_FLAGS) $(CPPFLAGS) $(C_AGENT_CFLAGS) $(PCRE_CFLAGS) $(CFLAGS) -c $< -o $@
