# PHP Agent Development Guide
This guide is intended to be both a beginner installation guide as well as a developer reference.

* [FAQ](#faq)
* [Prerequisites](#pre-requisites)
* [Take the agent for a spin](#take-the-agent-for-a-spin)
* [Development](#development)
* [Pull requests](#pull-requests)

## FAQ
*So what's this __nrcamp/nrlamp__ stuff?*
* For a long while, the PHP agent was tied to our homegrown [NRLAMP](https://source.datanerd.us/php-agent/nrlamp) and [NRCAMP](https://source.datanerd.us/php-agent/nrcamp) stack. The NRLAMP stack contains Apache, Mysql, PHP, and related tools.

*What still builds against __nrcamp/nrlamp__?*
* The build jobs in Jenkins.

*Historically speaking, why would one have used this unwieldy and large installation?*
* It's closest to the code we build for customers (and it's required to run the pull request builder locally).
* It's got a pretty convenient method for switching between versions of PHP.

*Do I have to develop with it?*
* Nope! In fact, the team is currently turning away from using this **nrcamp/nrlamp** stuff and replacing it with the work available in the [php-agent/php-build-scripts](https://source.datanerd.us/php-agent/php-build-scripts) repository.  In fact, if you are working on building out a new development environment, go [there](https://source.datanerd.us/php-agent/php-build-scripts) first and follow the README as a first step. The php-build-scripts will construct a collection of PHP installs.

*How do I specify and switch between versions of PHP if I used the php-build-scripts?*

That depends on how deep you want to go! After you've got your PHP versions installed into `/opt/nr/php` (by [following the instructions in the README](https://source.datanerd.us/php-agent/php-build-scripts)), you'll need to set the `PHP_VERSION` shell variable, and make sure the version's `bin` folder is in your shell `PATH`.  If you're the sort who likes manually editing your `.bash_profile` or `.bashrc`, you can do this with something like

```
export PHP_VERSION=5.6.30
export PATH=/opt/nr/php/$PHP_VERSION/bin/:$PATH

```

If you don't like fiddling with you bash environment variables manually, the [`phpvm`](https://source.datanerd.us/php-agent/php-build-scripts/blob/master/bin/phpvm.bash) script can do this for you.

    $ phpvm list
    Available Verions (from /opt/nr/php)
    5.3.29
    5.3.29-zts
    5.4.45
    5.4.45-zts
    5.5.38
    5.5.38-zts
    5.6.30
    5.6.30-zts
    7.0.19
    7.0.19-zts
    7.1.5
    7.1.5-zts

    $ phpvm change 7.1.5
    Current $PHP_VERSION: 7.1.5

    Current `php -v`
    PHP 7.1.5 (cli) (built: Aug 15 2017 20:05:01) ( NTS ) Copyright (c) 1997-2017 The PHP Group Zend Engine v3.1.0, Copyright (c) 1998-2017 Zend Technologies

    Current `which php`
    /opt/nr/php/7.1.5/bin/php

    Current $PATH
    /opt/nr/php/7.1.5/bin:/home/ubuntu/bin:/home/ubuntu/.local/bin:/opt/nr/php/7.0.19/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/go/bin

For a full report on `phpvm`'s functionality, try `phpvm help`.

**Important**: The **only** thing `phpvm` does is set the `PHP_VERSION` and `PATH` variables. You'll still need to get `newrelic.ini` files into each individual PHP folder, and make any necessary changes to your web server, `mod_php`, or `php-fpm` setups to run PHP from the new folder.

If you're not using the [`php-agent/php-build-scripts`](https://source.datanerd.us/php-agent/php-build-scripts) project yet, there's a script to helps switch between php versions in the **nrcamp/nrlamp** scenario.

```
#!/bin/sh
: "${NRLAMP_PHP:=${PHP_VERSION}}"
: "${NRLAMP_ZTS:=}"
exec "/opt/nr/php/${NRLAMP_PHP}/bin/php "$@"
```

## Prerequisites
### A Development Platform
If you want to work on Mac OSX, Homebrew should handle most of the installs you need.  That said, at the time of this writing, most of the team uses Linux in a dual-boot environment, in a vagrant box, or on an EC2 box.  For such platforms, use your Linux package manager of choice.

### Go!
Install the [latest version of Go](https://golang.org/dl/). Typically it's installed in `/usr/local/go`. Be sure to extend your **PATH** to include it!

### Submodules!
As noted in this repository's README.md, upon checkout, you will need to get the `cross_agent_tests` and `newrelic-integration` submodules. This can be done by running the command:
```
git submodule update --init
```

## Bye Bye NRCAMP and NRLAMP.
Here's where you choose whether to use the historical **nrcamp/nrlamp** setup, the newer php-build-scripts, or (on macOS only) Homebrew. The php-build-scripts are strongly recommended.

### The NRCAMP and NRLAMP route
Below are instructions for **nrcamp/nrlamp**.

Both NRCAMP and NRLAMP are installed into `/opt/nr`; this knowledge is burned in deeply. Prefix your search path to include `/opt/nr/camp/bin` and `/opt/nr/lamp/bin`.

#### System dependencies
Here are some non-exhaustive, system-specific dependencies we've encountered:

**Centos 6.5**

`yum install ncurses-devel gcc-c++ zlib-devel glibc-devel.i686 glibc-devel cyrus-sasl-devel`

|dependency|used for|
|----------|--------|
|ncurses-devel|NRCAMP texinfo|
|gcc-c++|NRCAMP gmp1|
|zlib-devel|NRCAMP gcc_bootstrap|
|glibc-devel.i686 glibc-devel|NRCAMP gcc_final|
|cyrus-sasl-devel|NRLAMP php70|

**Ubuntu 14**

`apt-get install ncurses-dev g++ zlib1g-dev libc6-dev-i386 libsasl2-dev`

|dependency|used for|
|----------|--------|
|ncurses-dev|NRCAMP texinfo|
|g++|NRCAMP gmp1|
|zlib1g-dev|NRCAMP gcc_bootstrap|
|libc6-dev-i386 libc6-dev|NRCAMP gcc_final|
|libsasl2-dev|NRLAMP php70|

#### Build it!
This step will take awhile (like, a few hours).
```
mkdir -p /opt/nr
chmod oug+rw /opt/nr
git clone git@source.datanerd.us:php-agent/nrcamp.git
git clone git@source.datanerd.us:php-agent/nrlamp.git

cd nrcamp
./getstuff
./buildit

cd ../nrlamp
./getstuff # on Yosemite, you may need --curl=/usr/bin/curl if nr-curl can't handle php7's https url
./buildit --valgrind --zend_debug --clean
```

### The php-build-scripts route
Follow the README in [php-agent/php-build-scripts](https://source.datanerd.us/php-agent/php-build-scripts) then come back here. You may need to manually create a directory:

```
$ mkdir -p /opt/nr/logs
```

### Homebrew

On macOS, you can also get the PHP versions you need via Homebrew's PHP tap. There are a couple of irritations, however:

1. To get embed support (required for the agent unit tests), you have to build PHP from source. This is, obviously, rather slower than pouring a bottle.
2. Having multiple versions installed at once is clunky, as `brew upgrade` invocations tend to demand that you unlink and re-link each PHP version in turn when there are newer versions.

Nevertheless, if you're really keen to do it this way, here's how you'd install PHP 7.2 with a reasonable set of extensions for development and testing:

```bash
brew tap php
brew install php72 --with-embed
brew install php72-gmp php72-intl php72-memcached php72-mongodb php72-opcache php72-pdo-pgsql php72-redis php72-uopz
```

For other versions, you can replace the `72` with whatever major and minor version you're interested in: eg `php53` for PHP 5.3.

Whichever version of PHP was installed or upgraded most recently will be the version at `/usr/local/bin/php`. You can control this with `brew unlink` and `brew link`. Alternatively, to build the PHP agent against a specific version of PHP, you can reference the cellar directory the version lives in (eg `make agent-check PHPIZE=/usr/local/Cellar/php72/7.2.1_12/bin/phpize PHP_CONFIG=/usr/local/Cellar/php72/7.2.1_12/bin/php-config` to build against php72 version 7.2.1_12).

## Build the PHP Agent
Hooray! Now let's build the agent. [Grab the agent](git@source.datanerd.us:php-agent/php_agent.git). Running `make` from the top level of the php agent directory builds the correct agent and daemon.
- If you've followed the **nrcamp/nrlamp** directions, `make` will use the PHP you specify, defaulting to version specified by the environment variable **NRLAMP_PHP**.
- If you utilized the **php_build_scripts**, take a moment to re-read the above FAQ section on _"How do I specify and switch between versions of PHP if I used the php-build-scripts?"_

Here are the non-exhaustive capabilities of the Makefile:

|command|function|
|-------|--------|
|`make -j8`|use 8 CPUs|
|`make agent`|just build the agent|
|`make agent-install`|build the agent and move it to the appropriate location|
|`make daemon`|build the daemon|
|`make run_tests`|build and run the agent and axiom unit tests (agent unit tests currently require Linux)|
|`make run_tests TESTS=test_txn`|run only the **test_txn.c** tests|
|`make run_tests SKIP_TESTS=test_rpm`|skip the **test_rpm.c** tests|
|`make axiom-valgrind`|build and run the axiom tests under valgrind|
|`make CC=/usr/bin/clang`|use own compiler|
|`make PHPIZE=/opt/php/5.5/bin/phpize PHP_CONFIG=/opt/php/5.5/bin/php-config`|use any PHP install|
|`make daemon_cover`|Go code coverage|
|`make daemon_bench`|run daemon performance tests|
|`make daemon_integration`|run the Go tests with the addition of **collector_integration_test.go**|
|`make coverage`| Create a code coverage report based on the unit tests and integration tests. [More information](#code-coverage)|

For more detail on the targets and variables the Makefile supports, see the
[make reference](make.md).

### INI settings
Create your **newrelic.ini** with the agent's template. For **nrcamp/nrlamp**, this is:
```
cat agent/scripts/newrelic.ini.template agent/scripts/newrelic.ini.private.template > /opt/nr/etc/php-common/newrelic.ini`
```

For the **php-build-scripts** route, it is necessary to create a .ini file for each version of php. One approach is to create a .ini file in `/opt/nr/etc/php-common/newrelic.ini` and symbolically link the .ini file in, say, `/opt/nr/etc/php-5.6`.

Edit your **newrelic.ini** to add a license key, a unique application name, and a full path to your daemon. Make sure your log directory exists and is read/writable; `/opt/nr/logs` or `/var/log/` are good options. You may want to change the log levels.

Here are some suggested settings to change:
```
newrelic.license = "<my_license_key>"
newrelic.logfile = "/opt/nr/logs/php_agent.log"
newrelic.loglevel = "verbosedebug"
newrelic.appname = "Super_awesome_app"
newrelic.daemon.logfile = "/opt/nr/logs/newrelic-daemon.log"
newrelic.daemon.loglevel = "debug"
newrelic.daemon.location = "/Users/earnold/workspace/php_agent/bin/daemon"
newrelic.daemon.collector_host = "staging-collector.newrelic.com"
```

## Take the agent for a spin
Make sure the new **newrelic.so** you just created is somewhere PHP can see it. If you're using **nrcamp/nrlamp**, use `make agent-install` otherwise, just copy or symlink `agent/modules/newrelic.so` to the PHP you want to use.

### phpinfo()
Request `phpinfo()` in the terminal and watch the activity in the daemon and agent logs, then check your staging account. ZOMG, look at all those non-web background transactions!
```
php -i # do this a bunch of times to fully connect
tail -f /opt/nr/logs/php_agent.log
tail -f /opt/nr/logs/newrelic-daemon.log
```

### Apache
[**nrcamp/nrlamp** version] Replace the default **http.conf** with the nrlamp version and edit to taste.
```
cp /opt/nr/etc/httpd/httpd.conf.nrlamp /opt/nr/etc/httpd/httpd.conf
```
To enable request queue reporting, add `RequestHeader set X-Request-Start "%t"` at the top of the file.

The nrlamp-flavored document root is `/opt/nr/htdocs` and the default port is 9090. Create a phpinfo script in the htdocs directory and start up apache.
```
echo "<?php phpinfo(); ?>" > /opt/nr/htdocs/info.php
apachectl start
```
Navigate to <http://localhost:9090> in the browser to check it out. Look for activity in the daemon and agent logs, then check your staging account to see the data!

### Start the daemon manually
You can start the daemon manually or via the agent (the two previous examples had the agent start the dameon). To start it manually, tell the daemon not to launch automatically in your **newrelic.ini** with `newrelic.daemon.dont_launch = 3` and start it with appropriate flags:
```
./bin/daemon -f -logfile stdout -loglevel debug
```

## Development
### Unit tests
First, poke git to update the submodule for the cross-agent tests, then run the unit tests.
```
git submodule init && git submodule update
make run_tests
```

### Integration tests
```
./bin/integration-runner
```

### Valgrind
You can run `make valgrind` to build and run the axiom tests under Valgrind. If using **nrcamp/nrlamp**, ensure you've built nrlamp with the appropriate flags (see above). Export the following environmental variables to tell PHP to notify you of leaked zvals.
```
export USE_ZEND_ALLOC=0
export ZEND_DONT_UNLOAD_MODULES=1
```
There may also be **php.ini** setting(s) that are required. To test instrumentation, be sure to run valgrind with the full path to the PHP you want to use.
```
valgrind --leak-check=full /opt/nr/lamp/bin/php-5.6-no-zts <file>
```
### Code Coverage

#### Setup
You will need python and gcovr to generate the html report. Once you have 
installed python you can `pip install gcovr`.

#### Generating Reports
The makefile includes options for creating code coverage reports. The command: `make coverage` will 
compile with the coverage flags, run unit test and integration tests. The recipe will then create 
a summary coverage report named `coverage-report.html` as well as additional html files for each code 
file in the project. These files will contain contents of code files with annotations that summarize 
code coverage. 

Code coverage reports can also be generated with manual tests. The steps are as follows:
1. Compile with the code coverage flags `make ENABLE_COVERAGE=1`
1. Run your manual tests 
1. Create the coverage report
    * Create a detailed html report `make html-report`

Creating a coverage report can be generated for tests run on a single file:
1. Compile with the code coverage flags `make ENABLE_COVERAGE=1`
1. Run the single test
1. Create the coverage report
    * Create a detailed html report `make html-report`

## Pull requests
### Pull request builder
[Requires **nrcamp/nrlamp**] Did you know, your dev machine can probably run the pull request build job faster than Jenkins can! Try it out:
```
./hudson/build-pull-request.sh
```
The pull request script does the following:

1. Build the daemon & run integration tests for daemon
2. Build axiom & run its tests (on Linux, this is done under Valgrind)
3. Set up agent integration environment information
4. Build the no-zts agent for each version of PHP (5.3 to 7.3)
5. Run the agent integration tests for each version of PHP
6. Build the zts agent for each version of PHP

### Checklist
* [ ] Read your code. Look for errors, mistakes, and anything you may have missed.
* [ ] Run tests:
    * [ ] For C code, `make run_tests`
    * [ ] For Go code, `make daemon_test`
    * [ ] Run integration tests with `./bin/integration-runner`
    * [ ] If you worked on Go code relating to collector communication, run `make daemon_integration`
* [ ] If you worked on C code, run `make axiom-valgrind`, `make agent-valgrind`, or `make valgrind` (for both) to run the tests under valgrind as appropriate. Building with jemalloc can also be useful for finding memory leaks.
* [ ] If you use **nrcamp/nrlamp**, consider running the PR builder script
* [ ] If you added any secrets (credentials, keys, etc) to source code, ping the Security team.
* [ ] Check code coverage:
    * [ ] If you worked on C code, run `make ENABLE_COVERAGE=1`, run some PHP code, and `make gcov` to get coverage information on your tests. Try to get this as close to 100% as you can.
    * [ ] If you worked on Go code, run `make daemon_cover` for coverage information. Note that this only tests the newrelic package, so if you have installed your code in a subpackage, you will have to run coverage tests manually, with something like `go test newrelic/mypackage -coverprofile=profile.out`.
* [ ] If you think you may have impacted performance, `make daemon_bench` is available.
* [ ] If you have statically linked to or vendored non-New Relic code, ensure `LICENSE.txt` has been updated.

Be sure to check out our [code review best practices](code_review.md).
