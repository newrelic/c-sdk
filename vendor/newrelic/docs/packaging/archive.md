# PHP Agent: Tar Archives

In order to ensure the PHP Agent is available to as many platforms as
possible, the PHP Agent team publishes tar archives of the agent.

## Where are They?

The latest releases can be found at

> https://download.newrelic.com/php_agent/release/

with releases archived at

> https://download.newrelic.com/php_agent/archive/

The `download.newrelic.com` server is an Amazon S3 bucket that lives behind a
fastly cache. This is the same Amazon S3 bucket that can be accessed via
`apt.newrelic.com` and `yum.newrelic.com`.

We also have a testing server where the latest release builds are staged

> http://nr-downloads-private.s3-website-us-east-1.amazonaws.com/75ac22b116/
> php_agent/testing/

This is also an Amazon S3 bucket, but _not_ behind a fastly cache.

## What's in Them?

These archives contain everything a user needs to install their agent,
including (but not limited to)

1. 32-bit and 64-bit compiled versions of the PHP Agent for PHP 5.3, 5.4, 5.5, 5.6,
   7.0, 7.1, 7.2, and 7.3

2. 32-bit and 64-bit compiled versions of the daemon

3. "Init" shell scripts that start and stop the daemon

4. Scripts for \*nix `logrotate` systems that will rotate the daemon and agent logs

5. A template for a `newrelic.ini` file, the agent's configuration file

6. A template for a `newrelic.cfg` file, the daemon's configuration file

7. A compiled `newrelic-iutil` binary, used by the `newrelic-install` script

8. A `newrelic-install` script, used by customers to install the agent

We currently create tar archives for four platforms

1. Standard linux/libc
2. FreeBSD
3. OS X/MacOS/Darwin
4. "Musl" linux/libc (i.e. Alpine linux)

## Understanding `newrelic-install`

Customers can use the `newrelic-install` shell scripts to

1. Install the Agent
2. Uninstall the Agent
3. "Purge" the Agent

### Installing the Agents

When a user installs the agent using the tar archives, their intended
workflow is

1. Unarchive the Files
2. Run the `newrelic-install` shell script

This shell script has a number of different responsibilities.  Also, this
shell script is not exclusive to the tar archives -- both the Debian archives
and RPM archives use this installer as part of their package lifecycle
scripts.  In other words, just because you see something in
`newrelic-install`, doesn't mean it runs when a user runs the script
manually.

When a user runs the `newrelic-install` script manually, this script will
(not in this order)

- Locate PHP installations on system
- Identify the version(s) of PHP installed on the system
- Identify the chip architecture of the system (32-bit, 64-bit)
- If possible, identify the "os type" (Debian, RedHat, Alpine, etc.)
- Using the information gathered above
  - Symlink (or copy) the correct compiled PHP .so to the correct PHP folder
  - Copy the correct compiled daemon (32-but or 64-bit) to the correct system folder
  - Copy the daemon init or service script to the correct system folder
  - Copy the `newrelic.sysconfig` to the correct system folder
- Create the `/var/log/newrelic` folder
- Create the `/etc/newrelic` folder
- Create or update the daemon's newrelic.cfg file
- Create the PHP Agent's newrelic.ini file OR append to current `php.ini` if
  PHP's ini folders are disabled

### Uninstall the Agent

@todo -- not done

### Purge the Agent

@todo -- not done

## Source Repos and Jenkins Jobs

There's two stages to publishing the agent tarballs archives -- creating
them, and then uploading them to S3.

### Creating Archives

These tarball archives are created via Jenkins jobs.

- [`php-bigchange-tarballs`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-bigchange-tarballs/)
- [`php-master-tarballs`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-tarballs/)
- [`php-release-tarballs`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-release-tarballs/)

These are downstream jobs and usually not run manually.  i.e. An agent
engineer or automated process will kick off the controlling
[`php-bigchange-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-bigchange-agent),
[`php-master-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-agent)
or
[`php-release-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-master-agent)
job and that controlling job will kick off these downstream jobs.

The script that actually builds these jobs
[lives inside the PHP Agent repository](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/hudson/package.sh).

### Publishing Archives

Once created, the archives may be uploaded to S3 via the separate Jenkins job
named
[`php-publish-agent`](https://phpagent-build.pdx.vm.datanerd.us/view/All/job/php-publish-agent/).

The code that uploads an agent to S3 lives in a different repository --
specifically the
[`deploy-php.bash`script](https://source.datanerd.us/php-agent/php_deployment/blob/master/deploy-php.bash)
in the [`php-agent/php_deployment` repository](https://source.datanerd.us/php-agent/php_deployment).
