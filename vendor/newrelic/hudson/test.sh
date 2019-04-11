#!/bin/sh

#
# Test the agent under Jenkins, but should work equally well locally.
#
# Usage: test.sh [--optimize]
#
#   --optimize   Perform an optimized build with minimal debug info.
#                Default is no optimizations and full debug info.
#

set -e
set -u

optimize=0
test_agent='yes'
test_axiom='yes'
test_daemon='yes'

die() {
  if [ $# -gt 0 ]; then
    printf '\nFATAL: %s\n\n' "$*" >&2
  fi
  exit 1
}

if [ ! -d packaging ]; then
  printf 'FATAL: this script must be run from the root of the PHP agent project' >&2
  exit 1
fi

case ":$PATH:" in
  *:/opt/nr/camp/bin:*) ;;
  *) PATH=/opt/nr/camp/bin:$PATH
esac

case ":$PATH:" in
  *:/usr/local/go/bin:*) ;;
  *) PATH=/usr/local/go/bin:$PATH
esac

export PATH

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

#
# Parse args
#

for OPT in "$@"; do
  case $OPT in
    --agent)       test_agent='yes'  ;;
    --no-agent)    test_agent='no'   ;;
    --axiom)       test_axiom='yes'  ;;
    --no-axiom)    test_axiom='no'   ;;
    --daemon)      test_daemon='yes' ;;
    --no-daemon)   test_daemon='no'  ;;
    --optimize)    optimize=1 ;;
    --no-optimize) optimize=0 ;;
  esac
done

numcpus=$(hudson/ncpu.sh)
makeflags="-r -s -j ${numcpus:-3}"

if [ "$test_daemon" = 'yes' ]; then
  printf \\n
  printf '# Daemon integration tests\n'
  printf \\n

  make daemon_integration
fi

if [ "$test_axiom" = 'yes' ]; then
  printf \\n
  printf '# Axiom tests\n'
  printf \\n

  #
  # Limit valgrind to unoptimized builds on CentOS. On Alpine Linux, valgrind
  # appears to alter the results of floating point to string conversions
  # causing spurious test failures.
  #

  if [ "$(uname)" = 'Linux' ] && [ ! -e /etc/alpine-release ] && [ "$optimize" -eq 0 ]; then
    # shellcheck disable=SC2086
    make $makeflags OPTIMIZE=0 axiom-valgrind LDFLAGS='-Wl,--no-warn-search-mismatch -Wl,-z,muldefs'

  else
    # shellcheck disable=SC2086
    make $makeflags "OPTIMIZE=$optimize" axiom-run-tests
  fi
fi

# Run the agent integration tests without requiring any changes to the
# PHP installation, and (ideally) without the tests being negatively
# affected by any existing INI settings. For example, a pre-existing
# "extension = newrelic.so". For each version of PHP, override the INI
# file, INI directory and the extension directory using environment
# variables. The result is that we can run the tests concurrently
# from multiple jobs under Jenkins.
#
# TODO(aharvey): We should also look at running the agent unit tests, but doing
# so today is impossible as the build products are only shared libraries out of
# tree, and the agent unit tests must be built in tree. This may be better
# served as a downstream job that can spread across a matrix of nodes.

if [ "$test_agent" = 'yes' ]; then
  printf \\n
  printf 'TODO(msl): Agent integration tests.'
  printf \\n
fi

printf \\n
printf 'done\n'
