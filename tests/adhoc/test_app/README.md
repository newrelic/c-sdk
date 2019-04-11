# `test_app`

A useful tool to exercise most of the agent's functionality in one place.

## Building

`make` will build a `test_app` binary that can be run.

By default, the Makefile will look for the C agent at the project root, but you
can override this by providing the `LIBNEWRELIC_LIB` variable with the path to
the `libnewrelic.a` you would like to use. Similarly, `LIBNEWRELIC_INCLUDE` may
be used to override the include directory that will be used.

## Running

There are three environment variables that can change the behavior of this program.

    # the license key (required)
    export NEW_RELIC_LICENSE_KEY="...license key..."

    # setting your application name (required)
    export NEW_RELIC_APP_NAME="Your App Name"

    # Change the URL the agent sends data to (optional)
    export NEW_RELIC_HOST="...your-url.example.com..."

After setting these variables, running `./test_app` should result in a couple of
transactions being sent provided a daemon is running beforehand.  The attributes,
errors, segments, custom events, and metrics created by these transactions will be
visible in the standard New Relic APM UI.  Events and metrics will be queryable via
Insights and the metrics explorer. i.e. they are all standard New Relic Agent data
products.
