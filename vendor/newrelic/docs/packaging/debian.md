# PHP Agent: Debian Repository

In order to ensure customers using Debian linux and Debian variants (Ubuntu,
etc.) can using their standard system administration workflows, New Relic
publishes Debian packages and a Debian software repository for the PHP Agent.

## Where are They?

The Debian packages can be found at

> http://apt.newrelic.com/debian/dists/newrelic/non-free/

The `apt.newrelic.com` server is an Amazon S3 bucket that lives behind a
fastly cache.  This is the same Amazon S3 bucket that can be accessed via
`download.newrelic.com` and `yum.newrelic.com`.

We also have a staging server where the packages can be staged and tested.

> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/
> debian/dists/newrelic/non-free/

In both cases, navigating to folders below `non-free` will reveal the `amd64`
and `i386`  `.deb` packages and Debian repository metadata files needed to
run a Debian software repository.

The PHP Agent is made up of **three** Debian packages (and therefore three
`.deb` file)

- newrelic-daemon
- newrelic-php5
- newrelic-php5-common

See _Other Miscellany_ below for more information on how to use these
packages.

## What's in Them?

At a high level, the three Debian packages contain the same binary
deliverables as the
[archive files](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/archive.md),
just spread out over three packages.  The following
descriptions are not an _exhaustive_ description of the files contained in
each package.  If you need a full list of file, `.deb` packages are just unix
"`ar`" archives, and can be extracted via

    ar -x file-name.deb

### newrelic-daemon packages

The `newrelic-daemon` package contains a `newrelic-daemon` binary appropriate
for the chip architecture of the package (Debian Software Depositories have
separate `.deb` fields for 32-bit, 64-bit, etc).  This package also contains
a `logrotate` script for the daemon log file
(`/var/log/newrelic/newrelic-daemon.log`), an `init.d` initialization script,
an `/etc/default/newrelic-daemon` configuration file for the `init.d` script,
and a `newrelic.template.cfg` file.

### newrelic-php5 packages

This package contains the `.so` PHP extensions files for all supported PHP
versions that are appropriate for the package's chip architecture.

### newrelic-php5-common packages

The Common package contain a `logrotate` configuration file for the PHP Agent
log file (`/var/log/newrelic/php_agent.log`), a `newrelic.ini.template` file,
and a `configure` program written in python (see below).

## Understanding Debian Package Lifecycle and `newrelic-install`

In addition to containing the actual source, binary, and support files that
makeup a package, every Debian package includes a
[list of _control_files](https://en.wikipedia.org/wiki/Deb_\(file_format\)#Control_archive).
These control files may include scripts that run before and after the package
management client has installed the package files into the system, as well as
before and after the package management client removes those same files
during an uninstall.

The New Relic Debian packages attempts to use these control scripts to
install the agent without needing to rely on the same `newrelic-install.sh`
that ships with the tar archives -- the previously mentioned `configure`
python program is part of this.  However, depending on the version of PHP
that's installed, as well as the specific linux distribution, this may or may
not be possible, and the Debian installer scripts will fall back to telling
users to run `newrelic-install` manually.

## Source Repos and Jenkins Jobs

There's two stages to publishing the `.deb` archives -- creating the `.deb`
files, and then creating the repository metadata and uploading everything to
S3.

The Debian archive files are created via Jenkins jobs.

### Creating Archives

The following jenkins jobs create the `.deb` archive files.

- [`php-bigchange-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-bigchange-packages/)
- [`php-master-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-packages/)
- [`php-release-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-release-packages/)

These are downstream jobs and (usually) not run manually.  i.e. An agent
engineer or automated process will kick off the controlling
[`php-bigchange-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-bigchange-agent),
[`php-master-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-agent)
or
[`php-release-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-agent)
job and that controlling job will kick off these downstream jobs.

The code that actually does the building for the `php-release-*` jobs
[lives inside the PHP Agent repository](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/hudson/package.sh).
In addition to the standard binary build artifacts, this
build program uses a set of
[Debian specific files](https://source.datanerd.us/php-agent/php_agent/tree/master/packaging/debian)
to create the individual package metadata files, including control files like
`preinst`, `postinst`, etc.

### Publishing the Archives and Debian Repos

The code that publishes an agent
([`deploy-php.bash` program](https://source.datanerd.us/php-agent/php_deployment/blob/master/deploy-php.bash))
**does not** live in the `php-agent/php_agent` repository. Instead, this code lives in
the
[`php-agent/php_deployment`repository](https://source.datanerd.us/php-agent/php_deployment).

Publishing the `.deb` packages means

1. Uploading the `.deb` files generated by the `php-*-package` job
2. Regenerating the Debian repository metadata (using the `apt-ftparchive` command)

The program in `deploy-php.bash` does both.

## Other Miscellany

### Debian Repository URLs and Using Staging with `apt`

Per
[public New Relic documentation](https://docs.newrelic.com/docs/agents/php-agent/installation/php-agent-installation-ubuntu-debian),
end users need to add the following to
their `sources.list` in order to install the agent via Debian Repository
clients like `apt`, `apt-get`, etc.

> deb http://apt.newrelic.com/debian/ newrelic non-free

The is a standard Debian repository URL, and tells client programs like `apt`
that they an find packages at the following URL

> http://apt.newrelic.com/debian/dists/newrelic/non-free/

Because (from one point of view) all a Debian `source.list` does is construct
a URL to where the packages live, this means we have a staging version of the
Debian Software Repository.  This, in turn, means we can install versions of
the agents published to staging via `apt install` (and other Debian clients).
 All you need to do is add the following to your `sources.list` instead of
the `apt.newrelic.com` URL.

> deb
> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/
> debian/ newrelic non-free

and you'll be able to install packages from staging using tools like `apt`
