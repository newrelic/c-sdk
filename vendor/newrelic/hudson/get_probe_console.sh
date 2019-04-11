#! /bin/bash

#
# get the console output from all nodes for the PHP_system_probe job.
#
# rrh Fri Dec  6 09:18:13 PST 2013
#

BUILD_NO=lastSuccessfulBuild

if [ "$#" -ge 1 ] ; then
  BUILD_NO=$1
fi

LABELS=""
LABELS+=" centos5-32-nrcamp"
LABELS+=" centos5-32-toolbuilder"
LABELS+=" centos5-64-nrcamp"
LABELS+=" centos5-64-toolbuilder"
LABELS+=" macos106-64-nrcamp"
LABELS+=" macosx-10.8.4-toolbuilder"
LABELS+=" freebsd100-64-nrcamp"
LABELS+=" freebsd100-64-toolbuilder"

for label in ${LABELS} ; do
   /usr/bin/curl --silent https://pdx-hudson.datanerd.us/view/PHP/job/PHP_system_probe/label=${label}/${BUILD_NO}/consoleText
done
