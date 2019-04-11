if which docker.io > /dev/null; then
  DOCKER=docker.io
else
  DOCKER=docker
fi

DISTROS="debian-stable debian-testing lucid precise trusty"

build_container () {
  pushd `dirname "$0"`/../"$1"

  # We have to copy the output and scripts directories rather than symlinking
  # them due to Docker limitations: it won't follow symlinks within the
  # context directory.
  rm -rf output scripts
  cp -rv ../scripts .
  mkdir output
  cp ../../../newrelic-*deb output/
  sudo $DOCKER build -t "$1" .

  popd
}

run_test () {
  sudo $DOCKER run "$1" /scripts/docker-test.py | tee "`dirname "$0"`/../output/$1.tap"
}
