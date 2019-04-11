#!/bin/sh

#
# Usage: build-debian.sh [--arch-dependent-only]
#
# Options:
#
#   --arch-dependent-only   Skip architecture independent packages.
#                           Does not build newrelic-php5-common
#
# Expected Output:
#
#   releases
#   |-- newrelic-daemon_*_.i386.deb
#   |-- newrelic-daemon_*_.amd64.deb
#   |-- newrelic-php5_*_.i386.deb
#   |-- newrelic-php5_*_.amd64.deb
#   |-- newrelic-php5-common_*_.i386.deb
#   |-- newrelic-php5-common_*_.amd64.deb
#
# To test this script locally, run the following commands from the
# top-level of a clone of the PHP agent repository. Requires a Linux
# system capable of building the agent. Also requires the devscripts
# package to be installed (use apt-get install devscripts).
#
#   ./hudson/build.sh
#   ./packaging/build-debian.sh
#

set -e
set -u

if [ ! -d packaging ] || [ ! -d releases ]; then
  printf 'FATAL: this script must be run from the root of the PHP agent repository\n' >&2
  exit 1
fi

if [ ! -d releases/linux ]; then
  printf 'FATAL: a complete linux build is required to build DEB packages\n' >&2
  exit 1
fi

if ! command -v debuild >/dev/null 2>&1; then
  printf 'FATAL: debuild is required to build DEB packages\n' >&2
  exit 1
fi

# Whether to build architecture dependent only packages or all packages.
#
# When set to 'no', dpkg-buildpackage will invoke `debian/rules binary-arch`.
# When set to 'yes', dpkg-buildpackage will invoke `debian/rules binary`.
ARCH_DEPENDENT_ONLY=no

# Whether to run lintian on the resultant packages.
LINTIAN=no

# Disable signing of changes file and source packages, since we do not
# use or ship either.
DEBUILD_FLAGS='-uc -us'

for OPT in "$@"; do
  case $OPT in
    --arch-dependent-only)
      printf 'INFO: limiting to architecture dependent binary packages\n'
      printf 'INFO: the newrelic-php5-common package will be skipped\n'
      ARCH_DEPENDENT_ONLY=yes
      ;;
    *)
      echo >&2 "FATAL: unsupported option: ${OPT}"
      exit 1
      ;;
  esac
done

if [ "$ARCH_DEPENDENT_ONLY" = 'yes' ]; then
  DEBUILD_FLAGS="${DEBUILD_FLAGS} -B"
else
  DEBUILD_FLAGS="${DEBUILD_FLAGS} -b"
fi

if [ "$LINTIAN" = 'no' ]; then
  # debuild specific flags must come before dpkg-buildpackage flags.
  DEBUILD_FLAGS=" --no-lintian ${DEBUILD_FLAGS}"
fi

generate_changelog() {
  local version

  version=$(cat releases/linux/VERSION)

  cat <<EOF
newrelic-php5 (${version}) stable; urgency=low

  * See https://docs.newrelic.com/docs/php

 -- New Relic <support@newrelic.com>  $(date --rfc-2822)

EOF
}

# Substitute common shell functions into the given file and write to stdout.
#
#   $1 - name of the file to process
preprocess() {
  local infile
  local pkgname
  local common

  infile="$1"
  pkgname=$(printf '%s\n' "$1" | sed -e 's/\..*//')
  common=$(cat "${pkgname}.common")

  # Actually substitute the common file into the right place using awk.
  awk -v common="${common}" '/^#COMMON#$/ { print common; next } { print }' "${infile}"
}

# Initialize the build directory for debian packaging.
if [ ! -d releases/debian ]; then
  mkdir releases/debian
fi
rm -rf releases/debian/*

# Copy files that do not require preprocessing.
cp packaging/debian/compat \
   packaging/debian/control \
   packaging/debian/copyright \
   packaging/debian/rules \
   releases/debian/

cp packaging/debian/newrelic-daemon.default \
   packaging/debian/newrelic-daemon.postinst \
   packaging/debian/newrelic-daemon.postrm \
   packaging/debian/newrelic-daemon.preinst \
   packaging/debian/newrelic-daemon.prerm \
   releases/debian/

cp packaging/debian/newrelic-php5-common.preinst \
   packaging/debian/newrelic-php5.common \
   packaging/debian/newrelic-php5.preinst \
   packaging/debian/newrelic-php5.templates \
   releases/debian/

# Substitute common shell functions into files that need it.
for FILE in packaging/debian/*.in; do
  preprocess "$FILE" > "releases/debian/$(basename "$FILE" .in)"
done

generate_changelog > releases/debian/changelog

# debuild expects its working directory to have a debian subdirectory.
# shellcheck disable=SC2086
(cd releases; debuild $DEBUILD_FLAGS)

# The various dpkg tools are hardcoded to put their output into '..'. For
# this script, that's the top-level of the PHP agent repository. Move those
# files into releases/.
mv ./*.build ./*.changes ./*.deb releases/

# Clean up.
rm -rf releases/debian

printf \\n
ls -l releases

exit 0
