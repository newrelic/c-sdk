# PHP Agent: RedHat Package Management Repository

In order to ensure customers using RedHat linux and RedHat variants (CentOS,
Amazon Linux, etc.) can use their standard system administration workflows,
New Relic publishes RPM packages and an RPM software repository for the PHP
Agent.

## Where are They?

The RPM packages can be found at

> http://yum.newrelic.com/pub/newrelic/el5/

The `yum.newrelic.com` server is an Amazon S3 bucket that lives behind a
fastly cache.  This is the same Amazon S3 bucket that can be accessed via
`download.newrelic.com` and `yum.newrelic.com`.

We also have a staging server where the packages can be staged and tested.

> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/pub/newrelic/el5/

In both cases, the `i386` and `x86_64` sub-folders contain all the `.rpm`
packages and RPM Repository metadata files needed to run the RPM software
repository.

The PHP Agent is made up of **three** RPM packages (and therefore three
`.rpm` file)

- newrelic-daemon
- newrelic-php5
- newrelic-php5-common

See _Other Miscellany_ below for more information on how to install these
packages.

## What's in Them?

At a high level, the three RPM packages contain the same binary deliverables
as the
[archive files](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/archive.md),
just spread out over three packages.  The following
descriptions are not an _exhaustive_ description of the files contained in
each package.  If you need a full list of files in a particular package, the
`.rpm` packages are just unix "`cpio`" archives and may be extracted with the
`rpm2cpio` and `cpio` tools.

    $ rpm2cpio ./name-of-the.rpm | cpio -idmv

### newrelic-daemon packages

The `newrelic-daemon` package contains a `newrelic-daemon` binary appropriate
for the chip architecture (RPM Software Repositories have separate `.rpm`
files for 32-bit, 64-bit, etc).  This package also contains a `logrotate`
script for the daemon log file (`/var/log/newrelic/newrelic-daemon.log`), an
`init.d` initialization script, an `/etc/default/newrelic-daemon`
configuration file for the `init.d` script, and a `newrelic.template.cfg`
file.

### newrelic-php5 packages

This package contains the `.so` files for all PHP versions, and that are
appropriate for the package's chip architecture.

### newrelic-php5-common packages

The common package contain a `logrotate` configuration file for the PHP Agent
log file (`/var/log/newrelic/php_agent.log`), a `newrelic.ini.template` file,
and a copy of `newrelic-install.sh`.

## Understanding RPM Package Lifecycle and `newrelic-install`

An RPM archive contains _only_ the files distributed with the archive.
Unlike a
[Debian package](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/debian.md),
there are no control files.

However, in addition to these archive files, the `rpm` file format has
additional information "baked in" to the file format.  This includes scripts
to install and uninstall the packaged files.  You can view these scripts
using the `rpm` command line tool

    rpm -qp --scripts name-of-rpm.x86_64.rpm

These scripts will invoke the package's `newrelic-install` script and install
the agent in _all_ standard PHP locations.  However, its still suggested that
users run the `newrelic-install` program themselves in order to input their
app name and license key.

## Source Repos and Jenkins Jobs

There's two stages to publishing the `.rpm` archives -- creating the `.rpm`
files, and then creating the repository metadata and uploading everything to
S3.

The RPM archive files are created via Jenkins jobs.

### Creating Archives

The following jenkins jobs create the `.rpm` archive files.

- [`php-bigchange-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-bigchange-packages/)
- [`php-master-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-packages/)
- [`php-release-packages`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-release-packages/)

These are downstream jobs and usually not run manually.  i.e. An agent
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
[redhat specific files](https://source.datanerd.us/php-agent/php_agent/tree/master/packaging/redhat)
to create the individual package metadata files.  These files include
the `.spec` files, which will embed the install and uninstall scripts (along
with other metadata) into the `rpm` file itself.

### Publishing the Archives and RPM Repos

The code that publishes an agent ([`deploy-php.bash` program](https://source.datanerd.us/php-agent/php_deployment/blob/master/deploy-php.bash))
**does not** live in the `php-agent/php_agent` repository.
Instead, this code lives in the
[`php-agent/php_deployment` repository](https://source.datanerd.us/php-agent/php_deployment).

Publishing the `.rpm` packages means

1. Uploading the `.rpm` files generated by the `php-*-package` job
2. Regenerating the RPM repository metadata (using the `createrepo` command)
and uploading it

The program in `deploy-php.bash` does both.

## Other Miscellany

### RPM Repository URLs and Using Staging with `yum`

Per [public New Relic documentation](https://docs.newrelic.com/docs/agents/php-agent/installation/php-agent-installation-aws-linux-redhat-centos),
end users need to tell their `rpm` system about one of the following repository URLs

> sudo rpm -Uvh
> http://yum.newrelic.com/pub/newrelic/el5/i386/newrelic-repo-5-3.noarch.rpm
> sudo rpm -Uvh
> http://yum.newrelic.com/pub/newrelic/el5/x86_64/newrelic-repo-5-3.noarch.rpm

The is a standard RPM repository URL, and the `newrelic-repo-5-3.noarch.rpm`
file contains a list of package URLs.

The publishing scripts also publish a full repository to staging, which means
you can use the following URLs to install staging packages on your system

> sudo rpm -Uvh
> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/
> pub/newrelic-testing/el5/i386/newrelic-repo-5-3.noarch.rpm
> sudo rpm -Uvh
> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/
> pub/newrelic-testing/el5/x86_64/newrelic-repo-5-3.noarch.rpm

