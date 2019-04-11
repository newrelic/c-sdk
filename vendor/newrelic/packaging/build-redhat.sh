#!/bin/sh

#
# Usage: build-redhat.sh [--arch-dependent-only]
#
# Options:
#
#   --arch-dependent-only   Skip architecture independent packages.
#                           Does not build newrelic-php5-common
#
# Build the RPM packages for the PHP agent. This script should be run
# from the top-level of the PHP agent source repository.
#
# Expected Output:
#
#   releases
#   |-- newrelic-daemon-*-1.i386.rpm
#   |-- newrelic-daemon-*-1.x86_64.rpm
#   |-- newrelic-php5-*-1.i386.rpm
#   |-- newrelic-php5-*-1.x86_64.rpm
#   |-- newrelic-php5-common-*-1.i386.rpm
#   |-- newrelic-php5-common-*-1.x86_64.rpm
#
# To test this script locally, run the following commands from the
# top-level of a clone of the PHP agent repository. Requires a Linux
# system capable of building the agent. Also requires the rpm-build
# package to be installed (use yum install rpm-build).
#
#   ./hudson/build.sh
#   ./packaging/build-redhat.sh
#

set -e
set -u

if [ ! -d packaging ] || [ ! -d releases ]; then
  printf 'FATAL: this script must be run from the root of the PHP agent project\n' >&2
  exit 1
fi

if [ ! -d releases/linux ]; then
  printf 'FATAL: a complete linux build is required to build RPM packages\n' >&2
  exit 1
fi

if ! command -v rpmbuild  >/dev/null 2>&1; then
  printf 'FATAL: rpmbuild is required to build RPM packages' >&2
  exit 1
fi

# Always build agent and daemon architecture dependent packages.
specs='packaging/redhat/newrelic-daemon.spec packaging/redhat/newrelic-php5.spec'

ARCH_DEPENDENT_ONLY=no

for OPT in "$@"; do
  case $OPT in
    --arch-dependent-only)
      printf 'INFO: limiting to architecture dependent binary packages\n'
      printf 'INFO: the newrelic-php5-common package will be skipped\n'
      ARCH_DEPENDENT_ONLY=yes
      ;;
    *)
      printf 'FATAL: unsupported option: %s\n' "$OPT" >&2
      exit 1
      ;;
  esac
done

if [ "$ARCH_DEPENDENT_ONLY" = 'no' ]; then
  specs="packaging/redhat/newrelic-php5-common.spec ${specs}"
fi

# Initialize the required directory structure for redhat packaging.
mkdir releases/redhat
mkdir releases/redhat/BUILD
mkdir releases/redhat/RPMS
mkdir releases/redhat/SOURCES
mkdir releases/redhat/SPECS
mkdir releases/redhat/SRPMS

# TODO(msl): Why not just pass all of the spec files to one invocation of rpmbuild?
for spec in $specs; do
  pkgname=$(basename "$spec" .spec)

  printf \\n
  printf 'INFO: building %s\n' "$pkgname"
  printf \\n

  # TODO(msl): Sign the package as it's built with --sign
  rpmbuild -bb "$spec"
  screen -D -m /usr/bin/expect -f packaging/redhat/rpmsign.expect releases/redhat/RPMS/${pkgname}-[0-9]*.rpm
done

printf \\n
printf 'INFO: verifying package signatures\n'
printf \\n
rpm --checksig releases/redhat/RPMS/*.rpm

# Move packages into releases/ and remove the temporary build directory.
mv releases/redhat/RPMS/*.rpm releases/
rm -rf releases/redhat
ls -l releases

exit 0
