#!/bin/bash

set -e
set -x

VALGRIND_VERSION=3.11.0

source /hbb_shlib/activate

curl -L http://valgrind.org/downloads/valgrind-$VALGRIND_VERSION.tar.bz2 | tar -C /tmp -jx
cd /tmp/valgrind-$VALGRIND_VERSION && ./configure
cd /tmp/valgrind-$VALGRIND_VERSION && make
cd /tmp/valgrind-$VALGRIND_VERSION && make install
