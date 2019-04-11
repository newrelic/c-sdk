# Introduction to Publishing the PHP Agent

As software engineers, it's easy to fall into the trap of thinking

> OK, we're code complete and tests are passing, this agent release is _done_

However, there's one last important step to consider -- *"How do we get the
agent into the hands of our current, and future, customers"*.

For agents targeting a single platform vendor, there's usually a straight
forward path.  For example, the [traditional .NET Agent](https://docs.newrelic.com/docs/agents/net-agent/installation/install-net-agent-windows#installing_framework)
has a clear path forward -- Microsoft
(famously) wants developers on its platforms, and provides various systems
that let users install software onto their system.  i.e. the traditional .NET
Agent provide customers with a `.msi` installer and be confidant that they're
reaching their target audience.

For agents targeting an open source language, things are a little more
complicated.  It's easy to say "let's target linux" -- but which linux are
you talking about?  There are subtle (and not so subtle) differences between
the various linux distributions that make creating a "one size fits all"
package a less than straight forward tasks.

One solution that agents take is piggy backing on their _language's_ package
or dependency management system.  For example, the ruby agent provides a
[ruby gem](https://rubygems.org/gems/newrelic_rpm/) that allows users to
[install the ruby agent](https://docs.newrelic.com/docs/agents/ruby-agent/getting-started/introduction-new-relic-ruby#compat).
The node agent provides [an npm package](https://www.npmjs.com/package/newrelic), etc.

Being able to take advantage of a language's package management system allows
agent developers to "outsource" the work of getting their code running on a
particular system to the language and package management system developers.
i.e. If users have a working version of ruby, node, etc. on their system,
they can adopt the agent.

## Why the PHP Agent is Still Stuck

Unfortunately, some of the challenges unique to the PHP Agent mean we _can't_
take advantage of the PHP's package management ecosystem.

PHP _does_ have a modern dependency manager in
[Composer](https://getcomposer.org/).   However, composer distributes code
written in PHP.  The PHP Agent is _not_ implemented  in PHP code -- the lack
of native threads and the lack of an API for redefining functions and methods
(i.e. monkey patching, hooking, etc.) make this impossible. Instead, the PHP
Agent is a PHP Extension, which means it's implemented in C code.

PHP has a _separate_ package management system for extensions called pecl.
Unfortunately, the PHP Agent can't take advantage of pecl.
[Gatekeeping policy around the central pecl repository](https://pecl.php.net/account-request.php)
is a little unclear, and strongly encourages (i.e. likely requires) an _open source_
license.  The PHP Agent is neither open source, or (as of this
writing) source available, which makes the central pecl repository a
non-starter.  Running our own pecl/pear channel _might_ be an option (this is
a bit of a dark art), but even that requires making the agent source
available.

As long as the PHP Agent distributes compiled binaries, the package
management systems provided by the PHP language won't be able to help us
distribute our agent.

## What the PHP Agent Publishes

We have multiple approaches to solve this tricky distribution/publication problem.

### Tar archives

As a lowest common denominator, we provide tarball archives that contain
every compiled version of the agent we support along with a
`newrelic-install` shell script that will install the agent.   The latest
release is always available to download via

> http://download.newrelic.com/php_agent/release/

with archive releases at

> http://download.newrelic.com/php_agent/archive/

For more details on what's included in these archives, read the [PHP Agent: Tar Archives](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/archive.md) doc.

### Popular Linux Package Management Systems

We also provide packages for the most popular linux distributions via a _Debian Software Repository_ hosting _Debian Packages_ and an
_RPM Repository_ hosting _RPM Packages_.

#### Debian Repository

The root of the Debian Repository (used by Debian linux variants like --
Debian, Ubuntu, etc.) can be found at

> http://apt.newrelic.com/debian/

and should be added to your source repo lists with the following

    deb http://apt.newrelic.com/debian/ newrelic non-free

To learn more about the Debian Repository, read the [PHP Agent: Debian Repository](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/debian.md) doc.

#### RPM Repository

The root of the RPM Repository (used by RedHat linux variant like -- RedHat,
CentOS, Amazon Linux, etc.) repo can be found at

> http://yum.newrelic.com/pub/

and can be added to your RPM repository lists with the following commands

    # for 32-bit
    sudo rpm -Uvh
    http://yum.newrelic.com/pub/newrelic/el5/i386/newrelic-repo-5-3.noarch.rpm

    # for 64-bit
    sudo rpm -Uvh
    http://yum.newrelic.com/pub/newrelic/el5/x86_64/newrelic-repo-5-3.noarch.rpm

To learn more about the RPM Repository, read the
[PHP Agent: Debian Repository](https://source.datanerd.us/astorm/php_agent/blob/packaging-docs/docs/packaging/rpm.md) doc.

## Home Grown Publishing

Between these two linux repositories and a the tarball distribution method,
any customer (or potential customer) will be able to grab and install the
PHP Agent on their systems.

This accessibility comes with a cost though: complexity.

There's enough feature drift in the various \*nix distributions that the
tarball, Debian Packages, and RPM Packages each have overlapping install and
uninstall mechanisms.  Also, differences in the `.rpm` package and `.deb`
package formats mean separate build scripts for each system.

Finally, PHP itself is a wildly popular system that's used in wildly
different ways.  There's no one true package repository for installing PHP
and its pecl extensions, and compiling PHP from source (eschewing package
management) remains a popular way for more tech savvy users to build their
systems.  All this means the various installer scripts need to a lot of work
to figure out the best ways to install the new relic agent into **any**
system.

It's a lot of extra work, but making the PHP Agent easy to access across a
variety of platforms is one of the reasons it's New Relic's most popular
agent.
