#!/bin/sh

set -e
set -u

#
# Figure out the available hardware parallelism
# See http://stackoverflow.com/questions/6481005/obtain-the-number-of-cpus-cores-in-linux
# for a discussion about both linux and macosx
#

numcpu=

# Try GNU coreutils' nproc utility first.
if command -v nproc >/dev/null 2>&1; then
  numcpu=$(command nproc 2>/dev/null) || numcpu=
fi

# Next, try some platform specific options.
if [ -z "$numcpu" ]; then
  case "$(uname -s)" in
    'Darwin')
      numcpu=$(sysctl -n hw.logicalcpu_max 2>/dev/null) || numcpu=
      ;;
    'FreeBSD')
      numcpu=$(sysctl -n hw.ncpu 2>/dev/null) || numcpu=
      ;;
    'SunOS'|'SmartOS')
      numcpu=$(/sbin/psrinfo -p 2>/dev/null) || numcpu=
      ;;
  esac
fi

# Finally, try the POSIX getconf utility.
if [ -z "$numcpu" ]; then
  numcpu=$(getconf _NPROCESSORS_ONLN 2>/dev/null) \
      || numcpu=$(getconf NPROCESSOR_ONLN 2>/dev/null) \
      || numcpu=
fi

test -n "$numcpu" && printf '%s\n' "$numcpu"
