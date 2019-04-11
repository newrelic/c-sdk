#! /bin/bash

#
# Set up the ruby environment using rbenv.
#
# THIS FILE MUST BE SOURCED USING THE "." COMMAND, NOT EXECUTED
#
# This presupposes that ruby has already been built using rbenv.
# The version of ruby used here must match what is built in this hudson job:
#   https://pdx-hudson.datanerd.us/view/PHP/job/PHP_build_ruby/
# which in turn runs the script
#   https://source.datanerd.us/newrelic/nrcamp/blob/master/buildruby.sh
#
PATH="$HOME/.rbenv/bin:$PATH"
eval "$(rbenv init -)"
rbenv shell 1.9.3-p448
if true; then
  which ruby
  ruby --version
fi
