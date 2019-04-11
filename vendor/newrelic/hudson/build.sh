#!/bin/sh

#
# Build the agent under Jenkins, but should work equally well locally.
#
# This script mixes the top-level Makefile build model with the legacy
# agent build model. Both need some common variables, such as the version
# number. The top-level Makefile is used as the single source of truth
# to ensure consistent output.
#
# Usage: build.sh [--optimize]
#
#   --optimize   Perform an optimized build with minimal debug info.
#                Default is no optimizations and full debug info.
#

set -e
set -u

printf \\n
printf 'HOST: %s @ %s\n' "$(uname -mrs)" "$(hostname)"

printf \\n
printf '%s\n' "$PATH" | tr ':' '\n' | sed 's/^/PATH: /'

printf \\n
if [ $# -gt 0 ]; then
  for ARGV in "$@"; do
    printf 'ARGV: %s\n' "${ARGV}"
  done
else
  printf 'no arguments given\n'
fi

die() {
  printf \\n
  printf 'FATAL: %s\n' "$*"
  printf \\n
  exit 1
}

test -d /opt/nr/camp || die 'NRCAMP not found'
test -d /opt/nr/lamp || die 'NRLAMP not found'

case ":$PATH:" in
  *:/opt/nr/camp/bin:*) ;;
  *) PATH=/opt/nr/camp/bin:$PATH
esac

case ":$PATH:" in
  *:/opt/nr/lamp/bin:*) ;;
  *) PATH=/opt/nr/lamp/bin:$PATH
esac

case ":$PATH:" in
  *:/usr/local/go/bin:*) ;;
  *) PATH=/usr/local/go/bin:$PATH
esac

export PATH

uname_system=$(uname -s)

if [ "$uname_system" = 'Darwin' ]; then
  # Minimum supported version of Mac OS X is 10.6.
  # Define this early to ensure all C sources (agent, axiom and installer)
  # are compiled the same way.
  export MACOSX_DEPLOYMENT_TARGET=10.6
fi

numcpus=$(hudson/ncpu.sh)
if [ -z "$numcpus" ]; then
  numcpus=3
fi

#
# Parse args
#

optimize=0
for OPT in "$@"; do
  case $OPT in
    --optimize)    optimize=1 ;;
    --no-optimize) optimize=0 ;;
  esac
done

# Delete any existing release products.
rm -rf releases

if [ "$uname_system" = 'SunOS' ] || [ "$uname_system" = 'SmartOS' ]; then
  # Solaris is a multi-arch system. Cross build both 32-bit and 64-bit products.
  make -r -j "$numcpus" release "OPTIMIZE=${optimize}" ARCH=x86

  # TODO(msl): Install utility doesn't rebuild correctly when ARCH changes.
  rm -f bin/newrelic-iutil
  make -r -j "$numcpus" release "OPTIMIZE=${optimize}" ARCH=x64
else
  # Build for the native arch everywhere else.
  make -r -j "$numcpus" release "OPTIMIZE=${optimize}"
fi

printf \\n
if command -v tree >/dev/null 2>&1; then
  tree releases
else
  ls -l releases
fi

printf \\n
printf 'done\n'
