#!/bin/sh

# Usage: package.sh
#
# Provide a common entry point for the Jenkins build jobs to create
# the redistributable packages for the PHP agent. This script only
# exists to document configuration specific to Jenkins in one place.
# This keeps the jobs themselves as simple as possible. The real work
# is delegated to the scripts in the top-level packaging directory.
#
# Expected Output (Tarballs):
#
#   releases
#   |-- newrelic-php5-*-freebsd.tar.gz
#   |-- newrelic-php5-*-linux.tar.gz
#   |-- newrelic-php5-*-linux-musl.tar.gz
#   |-- newrelic-php5-*-osx.tar.gz
#
# Expected Output (DEB Packages):
#
#  releases
#   |-- newrelic-daemon_*_.i386.deb
#   |-- newrelic-daemon_*_.amd64.deb
#   |-- newrelic-php5_*_.i386.deb
#   |-- newrelic-php5_*_.amd64.deb
#   |-- newrelic-php5-common_*_.i386.deb
#   |-- newrelic-php5-common_*_.amd64.deb
#
# Expected Output (RPM Packages):
#
#   releases
#   |-- newrelic-daemon-*-1.i386.rpm
#   |-- newrelic-daemon-*-1.x86_64.rpm
#   |-- newrelic-php5-*-1.i386.rpm
#   |-- newrelic-php5-*-1.x86_64.rpm
#   |-- newrelic-php5-common-*-1.i386.rpm
#   |-- newrelic-php5-common-*-1.x86_64.rpm
#
# Variables that affect this script:
#
#   JOB_NAME  - The name of the Jenkins job. For matrix projects this also
#               includes the node label.
#               Example: php-release-packages/label=centos5-64-nrcamp
#

set -e
set -u

if [ ! -d packaging ] || [ ! -d releases ]; then
  printf 'FATAL: this script must be run from the root of the PHP agent repository' >&2
  exit 1
fi

# Log the current host, environment and PATH. Environment variables that
# provide passwords or secret keys for remote services are filtered out.

echo
echo "HOST: $(uname -mrs) @ $(hostname)"

echo
env | sed \
          -e '/^PATH/d' \
          -e '/ACCESS_KEY=/d' \
          -e '/PASSWORD=/d' \
          -e '/SECRET_KEY=/d' \
          -e 's/^/ENV: /' \
    | sort

echo
echo "$PATH" | tr ':' '\n' | sed 's/^/PATH: /'

printf \\n
if [ $# -gt 0 ]; then
  for ARGV in "$@"; do
    printf 'ARGV: %s\n' "$ARGV"
  done
else
  printf 'no arguments given\n'
fi

# Clearly separate environment info from script output.
printf \\n
printf \\n

release_handles='linux linux-musl osx freebsd'

# TL;DR: We need to verify the build artifacts for the agent are from
# the same upstream build and revision of the agent for safety.
#
# Why?
#
# Jenkins copies the build artifacts from the upstream Agent build job
# into the workspace for this job. Because the Agent job is a matrix project,
# the build artifacts are copied separately from each label. When the
# downstream job (i.e. this one) is run manually, there is no upstream
# build so Jenkins copies the build artifacts from the last successful
# build instead. This would be fine except that Jenkins determines the
# last successful build on a per-label rather than per-job basis. This
# can lead to the following scenario.
#
#   1) Agent build succeeds for Linux
#   2) Agent build fails for OSX
#   3) Tarballs job is started manually
#   4) Jenkins copies the artifacts into our workspace
#      a) Last successful Linux build is #100
#      b) Last successful OSX build is also #99 (!!!!!)
#
# Each agent build records and archives the version number and commit SHA
# in the appropriate releases/ subdirectory. These are VERSION and COMMIT
# files respectively. Use these to detect the above scenario and fail the
# job.
check_versions() {
  version=
  last_version=
  release_handle=
  last_release_handle=
  srcdir=

  printf 'Checking all build artifacts are from the same agent build...'

  for release_handle in "$@"; do
    srcdir="releases/${release_handle}"
    version="$(cat "${srcdir}/VERSION")-$(cat "${srcdir}/COMMIT")"

    if [ -n "$last_version" ] && [ "$last_version" != "$version" ]; then
      printf >&2 \\n
      printf >&2 'FATAL: VERSION NUMBER MISMATCH\n'
      printf >&2 \\n
      printf >&2 '%s = %s\n' "$release_handle"      "$version"
      printf >&2 '%s = %s\n' "$last_release_handle" "$last_version"
      printf >&2 \\n
      printf >&2 'If running under Jenkins, check whether the most recent agent build failed.\n'
      exit 1
    fi

    last_version=$version
    last_release_handle=$release_handle
  done

  printf 'OK\n'
}

if [ -z "${JOB_NAME:-}" ]; then
  printf 'FATAL: JOB_NAME not specified\n' >&2
  exit 1
fi

case "$JOB_NAME" in
  #
  # Job:   Tarballs
  # Label: any
  #
  # Build the following packages:
  #
  #   newrelic-php5-*-freebsd.tar.gz
  #   newrelic-php5-*-linux.tar.gz
  #   newrelic-php5-*-linux-musl.tar.gz
  #   newrelic-php5-*-osx.tar.gz
  #
  php-*-tarballs)
    # Don't warn about word splitting, that's precisely what we want.
    #
    # shellcheck disable=SC2086
    {
      check_versions $release_handles
      packaging/build-tgz.sh $release_handles
    }
    ;;

  #
  # Job:   Packages
  # Label: centos5-32-nrcamp
  #
  # Build the following packages:
  #
  #   newrelic-daemon-*-1.x86_64.rpm
  #   newrelic-php5-*-1.x86_64.rpm
  #   newrelic-php5-common-*-1.x86_64.rpm
  #
  # Note: The choice to build the common package on this node is
  # arbitrary. It could just as easily (and correctly) be built on
  # a 64-bit node.
  #
  php-*-packages/label=centos5-32-nrcamp)
    packaging/build-redhat.sh
    ;;

  #
  # Job:   Packages
  # Label: centos5-64-nrcamp
  #
  # Build the following packages:
  #
  #   newrelic-daemon-*-1.x86_64.rpm
  #   newrelic-php5-*-1.x86_64.rpm
  #
  php-*-packages/label=centos5-64-nrcamp)
    packaging/build-redhat.sh --arch-dependent-only
    ;;

  #
  # Job:   Packages
  # Label: ubuntu10-32-nrcamp
  #
  # Build the following packages:
  #
  #   newrelic-daemon_*_i386.deb
  #   newrelic-php5_*_i386.deb
  #
  php-*-packages/label=ubuntu10-32-nrcamp)
    packaging/build-debian.sh --arch-dependent-only
    ;;

  #
  # Job:   Packages
  # Label: ubuntu10-64-nrcamp
  #
  # Build the following packages:
  #
  #   newrelic-daemon_*_amd64.deb
  #   newrelic-php5_*_amd64.deb
  #   newrelic-php5-common_*_amd64.deb
  #
  # Note: The choice to build the common package on this node is
  # arbitrary. It could just as easily (and correctly) be built on
  # a 32-bit node.
  #
  php-*-packages/label=ubuntu10-64-nrcamp)
    packaging/build-debian.sh
    ;;

  *)
    printf '\nFATAL: unsupported JOB_NAME=%s\n\n' "$JOB_NAME"
    ;;
esac

printf \\n
printf 'done\n'
