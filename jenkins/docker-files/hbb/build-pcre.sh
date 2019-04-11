#!/bin/bash

set -e
set -x

PCRE_VERSION=8.41

source /hbb_shlib/activate

curl -L https://ftp.pcre.org/pub/pcre/pcre-"$PCRE_VERSION".tar.bz2 | tar -C /tmp -jx
cd /tmp/pcre-"$PCRE_VERSION"
./configure --disable-shared --enable-static --prefix=/hbb_shlib
# hbb doesn't have nproc, so we'll brute-force it
make -j$(cat /proc/cpuinfo | grep processor | wc -l) install

cd /tmp
rm -rf pcre-"$PCRE_VERSION"
