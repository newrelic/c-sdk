#!/bin/sh

# Create test coverage reports based on the following:
# Build the daemon, agent and axiom tests in accordance with our pull request
# guidelines, which are documented in the README.md file. Additionally, run
# the axiom tests.
#
# This script is meant to run under Jenkins with the GitHub Pull Request
# Builder plugin enabled. The plugin handles reporting the outcome to
# GitHub for us.

set -e
set -u

die() {
  echo
  echo >&2 "FATAL: $*"
  echo
  exit 1
}

test -d /opt/nr/camp || die 'NRCAMP not found'
test -d /opt/nr/lamp || die 'NRLAMP not found'

case ":$PATH:" in
  *:/opt/nr/lamp/bin:*) ;;
  *) PATH=/opt/nr/lamp/bin:$PATH
esac

case ":$PATH:" in
  *:/opt/nr/camp/bin:*) ;;
  *) PATH=/opt/nr/camp/bin:$PATH
esac

case ":$PATH:" in
  *:/usr/local/go/bin:*) ;;
  *) PATH=/usr/local/go/bin:$PATH
esac

export PATH

numcpus=$(hudson/ncpu.sh)
if [ -z "$numcpus" ]; then
  numcpus=3
fi

printf \\n
printf 'Making daemon\n'
make ENABLE_COVERAGE=1 -r -s daemon

: "${MONGO_HOST:=phpai-helper-services.pdx.vm.datanerd.us}"
: "${MEMCACHE_HOST:=phpai-helper-services.pdx.vm.datanerd.us}"
: "${REDIS_HOST:=phpai-helper-services.pdx.vm.datanerd.us}"

export MONGO_HOST MEMCACHE_HOST REDIS_HOST

INTEGRATION_DIR="${PWD}/integration.tmp"
if [ ! -d "$INTEGRATION_DIR" ]; then
  mkdir "$INTEGRATION_DIR"
  mkdir "${INTEGRATION_DIR}/etc"
fi

rm -rf "${INTEGRATION_DIR:?}"/*

export PHPRC="${INTEGRATION_DIR}/php.ini"
export PHP_INI_SCAN_DIR="${INTEGRATION_DIR}/etc"

cat <<EOF >"$PHPRC"
date.timezone = "America/Los_Angeles"
extension_dir = "${PWD}/agent/modules"
extension = "newrelic.so"
newrelic.loglevel = "verbosedebug"
EOF

# If PHP is not defined use 7.3
[ -z "$PHP" ] && PHP=7.3

# Ensure NRCAMP and NRLAMP provided libraries can be found.
for LIBDIR in /opt/nr/[cl]amp/lib; do
  if [ -e "$LIBDIR" ]; then
   if [ -n "${LD_LIBRARY_PATH-}" ]; then
     LD_LIBRARY_PATH=$LIBDIR:$LD_LIBRARY_PATH
   else
     LD_LIBRARY_PATH=$LIBDIR
   fi
  fi
done

if [ -n "${LD_LIBRARY_PATH-}" ]; then
  export LD_LIBRARY_PATH
fi

printf "building agent (PHP=%s ZTS=disabled)\n" "$PHP"

for target in agent agent-check axiom-run-tests; do
  make ENABLE_MULDEFS=1 ENABLE_COVERAGE=1 -r -s -j "$numcpus" $target "NRLAMP_PHP=$PHP" NRLAMP_ZTS=
done
make integration PHPS="$PHP" INTEGRATION_ARGS="--retry=1"
make coverage-report-xml

printf \\n   # put a blank line before jenkins epilogue
