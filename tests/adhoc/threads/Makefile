AXIOM_INCLUDE := ../../../php_agent/axiom
LIBNEWRELIC_INCLUDE := ../../../include
LIBNEWRELIC_LIB := ../../../libnewrelic.a

CPPFLAGS := -I$(AXIOM_INCLUDE) -I$(LIBNEWRELIC_INCLUDE) $(shell pcre-config --cflags)
CXXFLAGS := -O0 -g3
LDFLAGS := -rdynamic
LIBS := $(LIBNEWRELIC_LIB) -lpcre -pthread -lboost_program_options -lm

all: threads

.PHONY: clean drd helgrind
clean:
	rm -f *.o threads

drd: threads drd-suppressions
	valgrind --tool=drd --suppressions=drd-suppressions ./threads -l $(NEW_RELIC_LICENSE_KEY) -h $(or $(NEW_RELIC_HOST),collector.newrelic.com)

helgrind: threads helgrind-suppressions
	valgrind --tool=helgrind --suppressions=helgrind-suppressions ./threads -l $(NEW_RELIC_LICENSE_KEY) -h $(or $(NEW_RELIC_HOST),collector.newrelic.com)

threads: main.o newrelic.o threads.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
