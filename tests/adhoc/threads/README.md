# Thread test

A basic stress test for our thread safety.

Every transaction created by this tool is created simultaneously on a different
thread. Each transaction then creates its own thread pool, with each thread
then creating random custom, datastore, or external segments until the desired
number is hit.

If you instrument this with valgrind, especially with `drd` or `helgrind`, you
get interesting results.

## Requirements

* The C agent.
* A compiler and standard C++ library that supports C++11.
* A recent-ish version of Boost; whatever ships on an up to date distro is
  probably fine.

## Building

`make` will build a `threads` binary that can be run.

By default, the Makefile will look for the C agent at the project root, but you
can override this by providing the `LIBNEWRELIC_LIB` variable with the path to
the `libnewrelic.a` you would like to use. Similarly, `LIBNEWRELIC_INCLUDE` may
be used to override the include directory that will be used.

## Running

`./threads --help` lists the various options. Only the licence key is
mandatory.

## Testing

There are helper targets in the `Makefile` for invoking `helgrind` and `drd`
with the appropriate suppressions. You will need to set the
`NEW_RELIC_LICENSE_KEY` variable when invoking `make`, and you may also want to
set the `NEW_RELIC_HOST` variable if you're not using the normal
`collector.newrelic.com` endpoint.
