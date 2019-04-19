include ../../../make/config.mk

LIBS := $(C_AGENT_ROOT)/libnewrelic.a 
LIBS += -lm 
LIBS += -lpthread 
LIBS += -lpcre

INCLUDES += -I$(C_AGENT_ROOT)/tests/integration/c-integration-test-helpers

C_AGENT_TEST_CFLAGS += '-DNEW_RELIC_DAEMON_TESTNAME="${NEW_RELIC_DAEMON_TESTNAME}"'
C_AGENT_TEST_CFLAGS += '-DNEW_RELIC_CONFIG=${NEW_RELIC_CONFIG}'

test_%: test_%.c
	$(CC) $(C_AGENT_CFLAGS) $(C_AGENT_CPPFLAGS) $(C_AGENT_TEST_CFLAGS) $(INCLUDES) -c $<
	$(CC) -o $@ $(<:.c=.o) $(LIBS)

clean_test_%:
	rm -f *.o $(subst clean_,,$@)

clean:
	rm -f *.log
