# PHP Agent Architecture
This document is a high-level overview of the basic architecture of the PHP
agent. It is intended to be useful for anyone trying to familiarize themselves
with the agent. For developers with a familiarity with a codebase this may be a
useful document for an overview, but more detailed information will be found by
reading the comments in the source.

## The Thousand-Yard View

1. The PHP agent comes in two parts: the PHP extension (agent) and the daemon
   (used to communicate with RPM).
2. Only UNIX environments are supported. Windows is not, and is currently
   highly unlikely to be.
3. We support all versions of PHP from 5.3 to 7.3, inclusive, both with and
   without Zend Thread Safety (ZTS). (We don't trust the ZTS versions, though;
   if you really want to support this it might be worth asking the team.)
4. Starting with version 3.0, licenses are configured via PHP itself, and can
   be changed on a per-directory or per-host basis.
5. You do not need super-user privileges in order to run either the agent or
   the daemon, although you do need to be able to edit a PHP ini file.
6. Pre-agent 5.0, we support up to 2047 simultaneous agents connecting to the
   daemon. Post-5.0, the number is limited only by the system's resources.
7. We run on Linux (kernel 2.6.14 or later, glibc 2.5 or later), OpenSolaris
   (snv_134b or later), FreeBSD-STABLE, and MacOSX (10.6 or later).
8. We provide native system packages for dpkg (Ubuntu, Debian etc) and rpm
   (RedHat, CentOS etc), as well as tar files.

## Agent, Daemon and Startup

The PHP agent isn't a single entity. It is split into two discrete parts: the
PHP extension which is loaded when PHP first starts up (by any means, be it on
the command line or inside Apache), and the New Relic daemon, which acts as a
communications proxy between the agent and our data collectors (the "RPM
site"). This is an important distinction. The actual PHP extension never, under
any circumstances, communicates directly with New Relic. The daemon is the only
entity that ever talks to the edge tier.

The main reason behind this separation is that PHP is most typically deployed
as a loaded module inside Apache, and Apache can be configured to create
hundreds (and in some cases thousands) of worker processes that deal with web
requests. The traffic from each of these processes communicating directly with
the collector would create a great deal more outbound traffic for the
customers, and place a great deal more stress on the collectors running in our
data centers. It would also make certain features (such as transaction traces)
very difficult to implement. The main downside to this implementation is that
there are two discrete components that need to be configured.

The daemon can be started in two modes:
 * "Daemonic" mode, where the daemon is started separately, usually by systemd
   or an init.d script; and
 * "Angelic" mode, where the agent checks to see if a daemon is running at
   startup, and if not, will spawn one.

When the agent is invoked for the first time, it will check to see if there is
a daemon running, and if not, it will automatically spawn a daemon process.
Daemon config settings can be set through agent INI file settings, and the
agent will pass those arguments to the daemon at runtime (overriding
newrelic.cfg settings). The "angelic" startup mode is the default, mostly
because it requires less setup on the user's part.

There are still cases where it is inadvisable for the agent to start the
daemon, which are discussed later in this document.

These two main components will now be discussed in more detail.

## The New Relic Daemon

This is a separate executable (or program) which is run outside of PHP and acts
as a proxy and aggregator between agents and the collector/edge tier. This
aggregation allows for considerable bandwidth savings in terms of amount of
data sent to the collector.

The daemon is configured via a file usually located in
`/etc/newrelic/newrelic.cfg`. The daemon settings can also be set via
`newrelic.daemon.*` settings in the PHP `newrelic.ini` file, and will be passed
on if the daemon is started by the agent.

The daemon actually has two roles: first, it provides various services to the
agents (remember, each Apache or php-fpm instance is a discrete agent, and
there can be many thousands of them), and second, it sends aggregated metrics
from all the agents to RPM.

### The Daemon and Agent Connect Requests

The RPM user interface allows users to set parameters such as high security
mode, in which we obfuscate many more parameters, overriding agent settings.
Because server-side configuration can change the runtime operation of the
agent, we must wait for a connect response ("appinfo") from the collector
before an agent run begins. Therefore, every time a web transaction starts,
the agent sends a connect request to the daemon with license and appname. If
the daemon has a cached appinfo reply, it sends that back to the agent, which
should be very quick. If the daemon does not have a cached reply, it sends a
not found response back to the agent, and that agent is disabled for a
variable timeout that increases with each failure. The daemon then asks the
collector about the application and caches the reply.

Once the daemon has received configuration information for an application, it
can begin responding in the positive to agents. Now when an agent starts up and
asks for its application settings, the daemon is able to reply with the full
response, and will never contact RPM again about the configuration information,
until such time as the daemon is either restarted, or RPM indicates that the
configuration information the agent currently has is "stale". This effectively
means that the daemon acts as a cache for all application configuration
information. This makes the startup time for each request in the agent
near-instantaneous.

The daemon will "forget" cached appinfo data after 10 minutes of no data
reporting. This means that users should see apps disappear when they have not
been reporting, but also means that apps with only a few long-lived processes
(for example, long-running scheduled jobs) will not appear in RPM at all: each
will cause the daemon to fetch an appinfo, but that information will
be dropped before the next run.

### The Daemon and Data Aggregation

The second service the daemon provides the agent is a "dumping ground" for
transaction data. In order to reduce the process that happens after each and
every request, the agent does as little as possible with the information it
gathered during the handling of a transaction. As soon as PHP indicates that
the transaction is complete, the agent does what little work it needs to do,
and then "dumps" the transaction to the daemon. When the daemon receives this
transaction dump, it does any required post-processing of the transaction data,
and merges (aggregates) the metrics received in the transaction dump into the
metrics received from other agent instances (or starts a new metric table if
this is the first). An advantage of this approach is that the daemon can choose
the "slowest SQL" or "slowest transaction trace", saving the agent from having
to compare those data across runs.

The daemon continues to gather transaction dumps from all agents, until it
reaches a "harvest cycle", which happens once every minute (20 seconds for a
few customers). When the harvest is run, all of the current metrics are sent up
to the collector and the cycle begins again.

The daemon now uses TLS (SSL) to communicate with the collector by default,
though this can be disabled by the user (why you would want to do this, I don't
know).

The daemon is separated into two processes, a "watcher" and its child process.
The child process does all the work, and the only function of the watcher is to
restart the child should it crash. This allows for increased uptime.

### Very High Performance

Prior to PHP Agent version 5.0, the daemon was written in C, and while it was
fast, it is hard to reason about multithreaded programs in C, and it is much
harder to maintain than a more modern language. In 5.0, the daemon was
completely rewritten in Go, allowing for greater maintainability and much
better support for concurrency. The new daemon is able to handle many more
agent connections (on the order of tens of thousands per minute).

### Two Modes of Operation / Launch

The daemon can be configured to start in one of two ways: either external to
the agent by a startup script, or automatically started by the agent when it
detects that no daemon is running. This latter mode is the default for all new
installations, and the former the default for all upgrades (as that was how it
was run in the past and the upgrade needs to preserve the status quo for the
user as much as possible). In case the terms appear in other literature these
two modes are referred to as "daemonic mode" (where the daemon is started
outside of the agent) and "angelic mode", where the daemon is started by the
agent. Some of the meaning of the original "Angelic" project has been lost
since it was named, but originally the thought was to get rid of the external
daemon completely, hence the name (removal of all daemons makes the system more
angelic).

There are distinct advantages and disadvantages to both modes of operation, and
selecting between them depends entirely on the system on which New Relic is
being deployed, what the users' permissions on that system are, and how the
system administrator chooses to configure the system. Since it has the longest
history the daemonic mode is discussed first.

#### Daemonic Startup

Daemonic mode requires that the daemon is configured with an external
configuration file, since it cannot get its configuration from the PHP config.
This file is typically `/etc/newrelic/newrelic.cfg`, though it can be any file
supplied by the user. The daemon does not need root privileges to run, and,
following the principle of least privilege, should generally not be run as
root. Typically in this mode the daemon is started as a service by the system
at boot time. We supply a unit file for systems using `systemd` (most modern
Linuxes) and a script in `/etc/init.d/` for systems not using systemd (BSD,
OSX, Solaris, older Linuxes).

Daemonic mode has some benefits. First, it makes it easier to restart the
daemon, which in high-throughput sites is a big issue. Secondly, as a shared
resource (even in a multi-tenant system all users can share the same daemon) it
means the daemon configuration need only be managed in one place. Lastly, it
may be more familiar to sysadmins.

#### Angelic Startup

Angelic mode requires the daemon be configured in the global PHP INI file. When
Apache, php-fpm or whatever first attempts to use PHP starts up, it will check
to see if there is an existing copy of the daemon running. If not, then the
agent will start it, using command line options whose values come from the INI
file. While this does offer the user considerable installation simplicity (not
to mention restricting all configuration to a single file), it does have its
drawbacks. The most important drawback and what system administrators need to
be aware of is that the daemon process will be "owned" by whichever user
happens to use PHP first. During a normal system boot sequence this will
certainly be the Apache or php-fpm user, which poses no problems, but should
the daemon ever terminate for any reason, then any random user that happens to
use PHP on the command line can cause the daemon to be started. This is highly
undesirable.

When using Angelic mode it is possible for the system administrator to restrict
exactly when the daemon is started by the agent via an INI file setting. This
can be restricted to non-command line, command line only, or never. This last
case would only ever be used if the daemon is being started in daemonic mode
and the cautious administrator wants to prevent accidental startup of the
daemon.

Despite these drawbacks, Angelic mode does have one very big advantage over
daemonic mode: ease of installation and configuration. Since all the
configuration is done in one place, this can provide a better experience for
the user. Since the agent will automatically start the daemon we eliminate the
support calls surrounding the agent being loaded into PHP but no metrics being
reported due to the daemon not having been started. For many small systems this
will provide a better experience than daemonic mode, simply because everything
"just works" out of the box, with no configuration required. This certainly
reduces the friction during the $2000 minute, and gets the user up and running.
Only if this method is somehow unacceptable to the user do they need to go and
change things, but chances are that will happen after they are already a paying
customer.

### Daemon Internal Architecture

The daemon runs a parent and child process. The parent supervises the child and
restarts it if it fails.

The child has two main goroutines: the Listener ([listener.go][]) and Processor
([processor.go][]). The Listener listens for network requests, and creates one
goroutine per agent process. The Processor aggregates data, and every minute,
sends a harvest to New Relic's edge tier. This multiple piece architecture
allows the daemon to take easy advantage of parallelism.

[listener.go]: /src/newrelic/listener.go
[processor.go]: /src/newrelic/processor.go

## The New Relic PHP Agent

The actual PHP agent, the part that instruments PHP code and gathers metrics,
is implemented as a PHP extension. Before going in to the specifics of the PHP
agent it is worth understanding a little bit about PHP itself, as PHP has some
wrinkles that aren't always obvious. Also since the terms may come up in
discussion it's worth understanding a little bit about the PHP architecture
itself.

PHP is a scripting language that is very popular for implementing web sites in.
It has many functions and features that make it ideal for web development, not
least of which is a large number of frameworks such as Drupal or Wordpress that
make it very easy for a casual user to set up a pretty powerful web site.

PHP as a language has some basic definitions and provides a number of basic
features, and then it has a plethora of language extensions.  These extensions
hook in to the internals of PHP and expand the language by adding new functions
and classes that are available for scripts to use. For example, a MySQL
extension would allow your script to access a MySQL database and query a
customer or product list. The vast majority of extensions do just that: simply
add to what appears to the user to be "the PHP language". However, extensions
can do far more than that, and can get their hooks in pretty deep into the
innards of the PHP engine itself, and the New Relic agent is one such
extension.

PHP itself is implemented on top of a bytecode engine known as the Zend engine.
This is not to be confused with a high level framework of the same name (the
Zend Framework is like Drupal, it provides a structured way of constructing
actual web sites and simply uses PHP as the language). Thus when you run a PHP
script, what happens behind the scenes is that the script gets compiled down to
bytecode (in much the same way that Java is) and then that bytecode is executed
by the Zend Engine (in much the same was as the JVM executes Java bytecode,
except that Zend doesn't provide JIT facilities). Just as the PHP language that
sits on top of it allows plugins to extend the PHP language, Zend can have
extensions too that allow plugins to extend the actual underlying engine
itself. The PHP agent does not currently use this facility, although there are
certain things that would be easier to implement if we did.

There are several versions of PHP in common use. PHP 5.3 is rather old, but we
have customers running on 5.3 through 7.3. We ship a different extension for
each PHP version, since we have to compile against different sources. The Zend
engine on top of which PHP is built can itself come in two flavours: with and
without Zend Thread Safety (ZTS). The PHP and Zend development teams both
highly discourage the use of Zend Thread Safety (ZTS) and they do not support
it at all. We supply ZTS versions of the agent, but do not officially support
them.

## Loading the PHP Agent

You need to tell PHP that you want to load any given extension. This is usually
as simple as editing a global INI file and adding a line telling it the name of
the extension to load. PHP extensions are usually all stored in the same
directory, and the agent install script will attempt to locate the correct
directory and place a copy of the agent in that directory. There are many
subtleties surrounding this, which are beyond the scope of this document. For
the purposes of this overview we will just assume the agent was installed
correctly.

When the PHP engine first starts up (this happens every time PHP is invoked on
the command line, and every time Apache or php-fpm are restarted) it goes
through the configured list of modules and loads them. Each module has a
special initialization function that is called that allows the extension to do
some global initialization. During this phase, the PHP agent will attempt to
contact the daemon and, if thus configured, start the daemon if it isn't
running. It does a bunch of other things too, some of which are briefly
described below, but the daemon initialization is the most important thing done
at this point. It also checks the various global INI options and will skip the
daemon initialization and will not install its various hooks into PHP and Zend
if the agent has been disabled for whatever reason. This phase of
initialization is called "module initialization" or "MINIT".

During the MINIT phase the agent registers a bunch of new PHP functions which
allow users to control the agent by means of normal PHP function calls.

### Request Initialization

Once all PHP extensions have completed their module initialization phase, the
PHP engine is considered "live" and can now begin processing requests. If you
are using PHP on the command line it will immediately begin a "request".
However, if you are using PHP inside Apache or php-fpm, a "request" is only
started when a user actually hits the web server involved and PHP is invoked to
deal with server connection. Stated slightly differently, when used on the
command line there is only ever a single "request", whereas when used in a
long-running process such as Apache or php-fpm, there can be thousands of
requests. In terms of the agent and New Relic vernacular, each such request is
a "transaction" or "web transaction".

As each request begins, the PHP engine gives all extensions a chance to do
per-request initialization. In our case, we use this hook to create a new web
transaction and set all metric counts to 0. This per-request initialization is
also sometimes referred to as the "RINIT" phase.

### Basic Agent Mechanics

When the agent is loaded into PHP, it hooks into the PHP and the Zend Engine in
order to be able to instrument PHP code. PHP provides a mechanism for an
extension to be called each time a PHP function is called, and the agent uses
this facility to effectively wrap each and every function call with the
following simple logic:

```
start_time = now ();
call_original_function ();
end_time = now ();
duration = end_time - start_time;
report_metric (duration);
```

That is, at a very high level, all the agent does. The devil, as they say, is
in the details. Some functions do more than just simple execution timers. If we
detect that the function being called is a MySQL function, then we dig into the
arguments to that function to get the actual SQL being sent to the database. Or
if we detect that its a MemCache call we generate MemCache metrics etc.

For information that is more in depth, see
[`agent/php_execute.c`](../agent/php_execute.c).

### Request Termination

When the script is completed, the PHP engine again calls all extensions letting
them know that. This is called the "RSHUTDOWN" phase and it gives extensions
the opportunity to clean up and release any resources it allocated when the
request started.

At this point, the transaction ends. This freezes the transaction name, if that
hasn't already occurred, and then starts the process of building the data
products to be sent to New Relic.

Firstly, the tree of segments within the transaction is iterated in full. All
segments have their exclusive time calculated, and metrics associated with
individual segments are created. Furthermore, the total time of the transaction
is calculated: unlike the duration (which is the wallclock time taken from the
start of the transaction to the end), the total time is the sum of the
exclusive time for every segment. If asynchronous or multi-threaded processing
has occurred, then the total time will usually be greater than the duration.

If the duration of the transaction is long enough to be eligible for a
transaction trace, then the slowest 2,000 segments are placed into a set.
Simultaneously, the most interesting 1,000 segments are added to a span event
set.

Once the total time has been calculated, duration and total time metrics,
intrinsics, and attributes are created on the transaction.

If span events or a transaction trace need to be generated, then another pass
over the segment tree is performed. This pass writes the JSON for the
transaction trace for segments that were in the trace set, and span events are
added to the span event reservoir based on the segments in the span event set.
As some span events may be omitted, span events created at this stage are
parented to the closest ancestor that is in the span event reservoir.

Once this process is complete, the agent sends the metric tables, events,
transaction trace, and error trace to the daemon, encoded in Flatbuffers.

Finally, the agent releases all the memory associated with the transaction, and
the transaction is done.

### PHP Termination

Finally, when PHP itself is about to shut down (either because the command line
script has now executed and PHP is about to exit, or because Apache or php-fpm
is being shut down), it calls all extensions letting them know that, and they
can do any last minute cleanup they need to. This is called the "MSHUTDOWN"
phase, and all our agent does here is contact the daemon to let it know it is
shutting down so that the daemon can keep accurate counts of how many agents
are actually "live".

And that is the PHP agent in a nutshell. Please do not hesitate to ask anyone
in the PHP team for more details if you need or want them.

### Source Code Layout

Source is in the `agent/`, `axiom/`, and `src/` directories. Axiom is meant to
be a PHP-independent library that could be used by other projects; at one point
it was shared with Linux Server Monitoring. Docs (like this) are in `docs/`.
Code in `src/` is Go code for our test runner and daemon; while working on the
daemon, set your `GOPATH` to the `php_agent` directory for Go tools to work
properly.
