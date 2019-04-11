# Set the usual bash settings for basic sanity.
set -e
set -x

# JaaS seems to give us an incomplete PATH, so we'll just set everything we
# need here.
export PATH=/usr/local/go/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH

# Activate the Holy Build Box in its shared library version.
source /hbb_shlib/activate

# Shim nproc, since Holy Build Box doesn't provide it.
nproc () {
  cat /proc/cpuinfo | grep processor | wc -l
}
