#!/bin/sh

# Build the daemon, agent and axiom tests in accordance  with our pull request
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
printf 'running daemon tests\n'
make -r -s daemon daemon_integration

#
# Limit valgrind to just the CentOS builders. On Alpine Linux, valgrind
# appears to alter the results of floating point to string conversions
# causing spurious test failures.
#

if [ "$(uname)" = Linux ] && [ ! -e /etc/alpine-release ]; then
  do_valgrind=yes
  printf \\n
  printf 'grinding axiom tests\n'
  make -r -s -j "$numcpus" axiom-valgrind
else
  do_valgrind=
  printf \\n
  printf 'running axiom tests\n'
  make -r -s -j "$numcpus" axiom-run-tests
fi

# Run the agent integration tests without requiring any changes to the
# PHP installation, and (ideally) without the tests being negatively
# affected by any existing INI settings. For example, a pre-existing
# "extension = newrelic.so". For each version of PHP, override the INI
# file, INI directory and the extension directory using environment
# variables. The result is that we can run the tests concurrently
# from multiple jobs under Jenkins.

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

# if PHPS is defined in the environment use,
# its value otherwise test everything
PHPS=${PHPS:-7.3 7.2 7.1 7.0 5.6 5.5 5.4 5.3}

#
# Build and test each version of PHP with thread safety (ZTS) disabled.
#

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

for PHP in $PHPS; do
  printf \\n
  printf "building agent (PHP=%s ZTS=disabled)\n" "$PHP"
  make agent-clean
  make -r -s -j "$numcpus" agent "NRLAMP_PHP=$PHP" NRLAMP_ZTS=

  printf \\n
  printf "running agent integration tests (PHP=%s ZTS=disabled)\n" "$PHP"
  #make integration PHPS="$PHP" INTEGRATION_ARGS="--threads=${numcpus}"
  make integration PHPS="$PHP" INTEGRATION_ARGS="--retry=1"

  printf \\n

  # Run the agent unit tests (just on Linux, for now).
  if [ "$(uname -s)" = 'Linux' ]; then
    PHP_PREFIX=$(env NRLAMP_PHP="$PHP" NRLAMP_ZTS= php-config --prefix)
    PHP_SAPIS=$(env NRLAMP_PHP="$PHP" NRLAMP_ZTS= php-config --php-sapis)

    case $PHP_SAPIS in
      *embed*)
        if [ -n "$do_valgrind" ]; then
          printf 'grinding agent unit tests\n'
          make -r -s -j "$numcpus" agent-valgrind "NRLAMP_PHP=$PHP" NRLAMP_ZTS= LDFLAGS='-Wl,--no-warn-search-mismatch -Wl,-z,muldefs'
        else
          printf 'running agent unit tests\n'
          make -r -s -j "$numcpus" agent-check "NRLAMP_PHP=$PHP" NRLAMP_ZTS= LDFLAGS='-Wl,--no-warn-search-mismatch -Wl,-z,muldefs'
        fi
	;;
      *)
        printf 'skipping agent unit tests - embed SAPI not present\n'
        ;;
    esac
  else
    printf 'skipping agent unit tests - not Linux\n'
  fi
done

#
# Build each version of PHP with thread safety (ZTS) enabled to ensure
# it compiles cleanly. We don't run the integration tests because
# (empirically) many PHP extensions are flakey with ZTS enabled leading
# to spurious failures that are not agent bugs.
#

for PHP in $PHPS; do
  printf \\n
  printf "building agent (PHP=%s ZTS=enabled)\n" "$PHP"
  make agent-clean
  make -r -s -j "$numcpus" agent "NRLAMP_PHP=$PHP" NRLAMP_ZTS=1

  # Run the agent unit tests (just on Linux, for now).
  if [ "$(uname -s)" = 'Linux' ]; then
    PHP_PREFIX=$(env NRLAMP_PHP="$PHP" NRLAMP_ZTS=1 php-config --prefix)
    PHP_SAPIS=$(env NRLAMP_PHP="$PHP" NRLAMP_ZTS=1 php-config --php-sapis)

    case $PHP_SAPIS in
      *embed*)
        if [ -n "$do_valgrind" ]; then
          printf 'grinding agent unit tests\n'
          make -r -s -j "$numcpus" agent-valgrind "NRLAMP_PHP=$PHP" NRLAMP_ZTS=1 LDFLAGS='-Wl,--no-warn-search-mismatch -Wl,-z,muldefs'
        else
          printf 'running agent unit tests\n'
          make -r -s -j "$numcpus" agent-check "NRLAMP_PHP=$PHP" NRLAMP_ZTS=1 LDFLAGS='-Wl,--no-warn-search-mismatch -Wl,-z,muldefs'
        fi
	;;
      *)
        printf 'skipping agent unit tests - embed SAPI not present\n'
        ;;
    esac
  else
    printf 'skipping agent unit tests - not Linux\n'
  fi
done

#
# Remove the workspace (only on Centos) after each build. This insures that the build nodes
# will not run out of free space.
#


if [ "$(uname)" = Linux ] && [ ! -e /etc/alpine-release ]; then
  rm -rf $PWD
  printf 'workspace cleared'
else
  printf 'skipping workspace clear - not Centos'
fi

printf \\n   # put a blank line before jenkins epilogue
