#!/bin/sh

#
# Usage: build-tgz.sh [RELEASE_HANDLE...]
#
# Build the tarballs for the PHP agent. This script should be run
# from the top-level of the PHP agent source repository. The output
# of this script is placed in the releases directory.
#
# Expected Output:
#
#   releases
#   |-- newrelic-php5-${VERSION}-${RELEASE_HANDLE}.tar.gz
#
# To test this script locally, run the following commands from the
# top-level of the PHP agent project.
#
# Linux:
#   ./hudson/build.sh
#   ./packaging/build-tgz.sh linux
#
# Mac OS X:
#   ./hudson/build.sh
#   ./packaging/build-tgz.sh osx
#

set -e
set -u

die() {
  if [ $# -gt 0 ]; then
    printf '\nFATAL: %s\n\n' "$*" >&2
  fi
  exit 1
}

if [ ! -d packaging ] || [ ! -d releases ]; then
  die 'this script must be run from the root of the PHP agent repository'
fi

# Locate openssl so we can generate MD5 checksums.
OPENSSL=/opt/nr/camp/bin/openssl
[ -x "$OPENSSL" ] || OPENSSL=$(command -v openssl 2>/dev/null || true)
[ -x "$OPENSSL" ] || die 'openssl not found: is NRCAMP missing?'

#
# Build the redistributable tarball for the given platform. Artifacts
# are expected to organized in the manner of `make releases`.
#
#   $1 - platform release handle (freebsd, linux, osx or solaris)
#
build_tgz() (
  cd releases

  srcdir=$1
  version=$(cat "${srcdir}/VERSION")
  destdir="newrelic-php5-${version}-${1}"

  [ -d "$srcdir"  ] || die "directory not found: ${srcdir}"
  [ -n "$version" ] || die 'VERSION not set'

  # Remove any previous build products.
  rm -f "${destdir}.tar.gz"

  # Be conservative about what we include in redistributables.
  rsync -av --delete --delete-excluded \
        --include=/LICENSE.txt \
        --include=/README.txt \
        --include=/newrelic-install \
        --include=/agent/ \
        --include=/agent/*/ \
        --include=/agent/*/newrelic-*.so \
        --include=/daemon/ \
        --include=/daemon/newrelic-daemon.* \
        --include=/scripts/ \
        --include=/scripts/init.alpine \
        --include=/scripts/init.darwin \
        --include=/scripts/init.debian \
        --include=/scripts/init.freebsd \
        --include=/scripts/init.generic \
        --include=/scripts/init.rhel \
        --include=/scripts/init.solaris \
        --include=/scripts/newrelic-daemon.logrotate \
        --include=/scripts/newrelic-daemon.service \
        --include=/scripts/newrelic-iutil.* \
        --include=/scripts/newrelic-php5.logrotate \
        --include=/scripts/newrelic.cfg.template \
        --include=/scripts/newrelic.ini.template \
        --include=/scripts/newrelic.sysconfig \
        --include=/scripts/newrelic.xml \
        --exclude=* \
        "${srcdir%%/}/" "$destdir"

  # Generate MD5SUMS in GNU coreutils format, i.e. what md5sum produces.
  # Use openssl instead of md5sum for portability.
  (
    cd "$destdir"
    find . -type f -a ! -name MD5SUMS \
	| sed -e 's,^./,,' \
	| xargs "$OPENSSL" dgst -md5 -r >> MD5SUMS
  )

  chmod 0755 "${destdir}/agent" "${destdir}/daemon" "${destdir}/scripts"

  # Read-only files.
  chmod 0444 \
        "${destdir}/LICENSE.txt" \
        "${destdir}/MD5SUMS" \
        "${destdir}/README.txt" \
        "${destdir}/scripts/newrelic.cfg.template" \
        "${destdir}/scripts/newrelic.ini.template"

  # Executables.
  chmod 0755 "${destdir}/newrelic-install"
  find "${destdir}/agent"   -type f -a -name 'newrelic-*.so'     -exec chmod 0755 '{}' \+
  find "${destdir}/daemon"  -type f -a -name 'newrelic-daemon.*' -exec chmod 0755 '{}' \+
  find "${destdir}/scripts" -type f -a -name 'init.*'            -exec chmod 0755 '{}' \+
  find "${destdir}/scripts" -type f -a -name 'newrelic-iutil.*'  -exec chmod 0755 '{}' \+

  # Everything else.
  chmod 0644 \
        "${destdir}/scripts/newrelic-daemon.logrotate" \
	"${destdir}/scripts/newrelic-daemon.service" \
        "${destdir}/scripts/newrelic-php5.logrotate" \
        "${destdir}/scripts/newrelic.sysconfig" \
        "${destdir}/scripts/newrelic.xml"

  # Create the archive.
  printf \\n
  tar cvf - "$destdir" | gzip -9 > "${destdir}.tar.gz"
  rm -rf "$destdir"

  printf \\n
  ls -l "${destdir}.tar.gz"
)

if [ $# -eq 0 ]; then
  printf 'nothing to do\n'
  exit 0
fi

for release_handle in "$@"; do
  printf \\n
  printf 'Building tarball for %s...\n' "$release_handle"
  printf \\n

  build_tgz "$release_handle"
done

exit 0
