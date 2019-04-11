#!/bin/bash

set -euo pipefail

# Ensure we're at the root of the repository.
cd "$(dirname "$0")"/..
if [ ! -d .git ]; then
  echo 'Cannot find the root of the C agent repository; exiting.'
  exit 1
fi
repodir="$(pwd)"
prefix="newrelic-c-agent-$(cat VERSION)"

# Make a work directory.
workdir="$(mktemp -d)"

# Copy the agent into that directory.
echo -n 'Copying agent to working directory... '
cd "$workdir"
cp -r "$repodir" .
mv * "$prefix"
cd "$prefix"
echo 'done.'

echo -n 'Tidying up directory... '
# Clean up any build products.
make clean > /dev/null
git clean -fx > /dev/null

# Remove things that we don't want to include in source tarballs.
rm -rf .git php_agent/{agent,docs,hudson,packaging,repo-manager,suppressions,tests} tools release_checklist.md jenkins
find -name vendor.yml -delete
echo 'done.'

# Build the actual tarball.
echo -n "Building tarball at $repodir/$prefix.tar.xz ... "
cd ..
tar Jcf "$repodir/$prefix.tar.xz" "$prefix"
echo 'done.'

# Clean up.
echo -n 'Tidying up... '
cd "$repodir"
rm -rf "$workdir"
echo 'done.'

echo 
ls -l "$prefix.tar.xz"
echo 'Share and enjoy!'
