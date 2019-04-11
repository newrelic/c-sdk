#!/bin/bash

#bail if anything goes wrong
set -e

#Create a base file name, using defaults if
#we're not getting the variables from jenkins
VERSION=${VERSION:-X.X.X}
LEVEL=${LEVEL:-alpha}
ARCHITECTURE=${ARCHITECTURE:-x86_64}
PLATFORM=${PLATFORM:-linux} 
BASE_FILE_NAME=libnewrelic-cagent-v${VERSION}-$LEVEL-$ARCHITECTURE-$PLATFORM

#clean previous builds
rm -rf libnewrelic/
rm -f libnewrelic*.tar.gz

#create folder structure for tarball
mkdir -p libnewrelic/bin

#move previous build artifacts over 
mv ./vendor/newrelic/bin/daemon libnewrelic/bin/newrelic-daemon
mv libnewrelic.a libnewrelic/

#move files from github repository over
mv examples/ libnewrelic/examples/
mv include/libnewrelic.h libnewrelic/
mv GUIDE.md libnewrelic/
mv LICENSE.txt libnewrelic/

#archive and gzip the project
tar -cvf ${BASE_FILE_NAME}.tar libnewrelic 
gzip ${BASE_FILE_NAME}.tar
mv ${BASE_FILE_NAME}.tar.gz ${BASE_FILE_NAME}.tgz
