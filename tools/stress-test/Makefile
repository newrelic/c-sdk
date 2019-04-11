#
# Makefile for New Relic C-Agent Code Examples
#

OBJS = stress_app.o

# This Makefile presumes that the locations of libnewrelic.h and libnewrelic.a are one
# level above this example directory.
LIBNEWRELIC_A_LOCATION = ../..
LIBNEWRELIC_H_LOCATION = ../../include
AXIOM_LOCATION = ../../php_agent/axiom

all:	stress_app

stress_app:	stress_app.o ../../libnewrelic.a
	$(CC) -o $@ $< -rdynamic -L$(LIBNEWRELIC_A_LOCATION) -lnewrelic $(PCRE_CFLAGS) -L/usr/local/lib/ -lpcre  -pthread

%.o: %.c
	$(CC) -c $< -o $@ -I$(LIBNEWRELIC_H_LOCATION) -I$(AXIOM_LOCATION)

clean:
	rm -f stress_app *.o c_sdk.log
