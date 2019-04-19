LIBNEWRELIC_INCLUDE := ../../../include
LIBNEWRELIC_LIB := ../../../libnewrelic.a
NEWRELIC_EXAMPLES := ../../../examples/

CFLAGS := -O0 -g3
CPPFLAGS := -I$(LIBNEWRELIC_INCLUDE) -I$(NEWRELIC_EXAMPLES) $(shell pcre-config --cflags)
LDFLAGS := -rdynamic
LIBS := $(LIBNEWRELIC_LIB) common.o -lpcre -pthread -lm

all: test_app test_app_trigger

.PHONY: clean
clean:
	rm -r *.o test_app test_app_trigger

test_app: test_app.o common.o
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

test_app_trigger: test_app_trigger.o common.o
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

# based on the implicit rule, but looks for common.c above
common.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o common.o ../../../examples/common.c
