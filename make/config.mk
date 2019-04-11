THISDIR  := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))

#
# Set up the basic variables required to build the C agent. This is mostly
# knowing where we are in the filesystem and setting up the appropriate
# compiler flags.
#
C_AGENT_ROOT := $(abspath $(THISDIR)/..)

#
# The PHP agent's build system does a bunch of useful platform detection that
# we need both in general and to utilise axiom. Let's pull it in straightaway.
#
include $(C_AGENT_ROOT)/vendor/newrelic/make/config.mk

C_AGENT_CPPFLAGS := $(PLATFORM_DEFS)
C_AGENT_CPPFLAGS += -I$(C_AGENT_ROOT)/vendor/newrelic/axiom -I$(C_AGENT_ROOT)/include

C_AGENT_CFLAGS := -std=gnu99 -fPIC -DPIC -pthread
C_AGENT_CFLAGS += -Wall
C_AGENT_CFLAGS += -Werror
C_AGENT_CFLAGS += -Wextra
C_AGENT_CFLAGS += -Wbad-function-cast
C_AGENT_CFLAGS += -Wcast-qual
C_AGENT_CFLAGS += -Wdeclaration-after-statement
C_AGENT_CFLAGS += -Wimplicit-function-declaration
C_AGENT_CFLAGS += -Wmissing-declarations
C_AGENT_CFLAGS += -Wmissing-prototypes
C_AGENT_CFLAGS += -Wno-write-strings
C_AGENT_CFLAGS += -Wpointer-arith
C_AGENT_CFLAGS += -Wshadow
C_AGENT_CFLAGS += -Wstrict-prototypes
C_AGENT_CFLAGS += -Wswitch-enum

ifeq (1,$(HAVE_CLANG))
C_AGENT_CFLAGS += -Wbool-conversion
C_AGENT_CFLAGS += -Wempty-body
C_AGENT_CFLAGS += -Wheader-hygiene
C_AGENT_CFLAGS += -Wimplicit-fallthrough
C_AGENT_CFLAGS += -Wlogical-op-parentheses
C_AGENT_CFLAGS += -Wloop-analysis
C_AGENT_CFLAGS += -Wsizeof-array-argument
C_AGENT_CFLAGS += -Wstring-conversion
C_AGENT_CFLAGS += -Wswitch
C_AGENT_CFLAGS += -Wswitch-enum
C_AGENT_CFLAGS += -Wuninitialized
C_AGENT_CFLAGS += -Wunused-label
C_AGENT_CFLAGS += -Wno-typedef-redefinition
endif

#
# The OPTIMIZE flag should be set to 1 to enable a "release" build: that is,
# one with appropriate optimisation and minimal debugging information.
# Otherwise, the default is to make a "debug" build, with no optimisation and
# full debugging information.
#
ifeq (1,$(OPTIMIZE))
	C_AGENT_CFLAGS += -O3 -g1
	C_AGENT_LDFLAGS += -O3 -g1
else
	C_AGENT_CFLAGS += -O0 -g3
	C_AGENT_LDFLAGS += -O0 -g3 -rdynamic
endif

export C_AGENT_ROOT C_AGENT_CFLAGS C_AGENT_CPPFLAGS

