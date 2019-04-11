#!/bin/sh

set -e
set -u

printf \\n
printf 'HOST: %s @ %s\n' "$(uname -mrs)" "$(hostname)"
printf \\n
printf '%s\n' "$PATH" | tr ':' '\n' | sed 's/^/PATH: /'
printf \\n
printf 'ARGV: %s\n' "$@"
printf \\n

scanbuild() {
  scan-build -v -analyze-headers --use-analyzer="$(command -v clang)" -o reports/scanbuild "$@"
}

numcpus=$(hudson/ncpu.sh)
if [ -z "$numcpus" ]; then
  numcpus=3
fi

if [ ! -d reports ]; then
  mkdir reports
fi

if [ ! -d reports/scanbuild ]; then
  mkdir reports/scanbuild
fi

#
# Analyze the axiom tests first, followed by each version of PHP. Each
# invocation of scan-build should perform a full build to ensure all of the
# source is analyzed.
#

make -s -r axiom-clean
scanbuild make -s -r -j "$numcpus" tests

for PHP in ${PHPS:-7.3 7.2 7.1 7.0 5.6 5.5 5.4 5.3}; do
  make -s -r agent-clean PHPIZE="/opt/nr/lamp/bin/phpize-${PHP}-no-zts"

  scanbuild \
      make -s -r -j "$numcpus" agent \
      PHPIZE="/opt/nr/lamp/bin/phpize-${PHP}-no-zts" \
      PHP_CONFIG="/opt/nr/lamp/bin/php-config-${PHP}-no-zts"
done
