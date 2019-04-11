# New Relic PHP Agent Release Notes #

## Master ##

### End of Life Notices ###

### New Features ###

### Upgrade Notices ###

### Notes ###

### Bug Fixes ###

* When obfuscating SQL, comments are stripped without any loss of the SQL itself.

* Predis 0.8 commands that used the synchronous `executeCommand()` code path
  (for example, `HSET`) on a clustered connection did not generate metrics.
  This has been fixed.

### Internal Changes ###

* Nodes are no more!

  Segments are now represented internally using a segment data type that
  provides better support for distributed tracing, asynchronous execution, and
  metric creation.

### Acquia-only Notes ###

## Contents ##

| Release Number | Release Date |
| ------------- |-------------|
| [8.6.0](#860) | 2019-03-14 |
| [8.5.0](#850) | 2018-12-19 |
| [8.4.0](#840) | 2018-12-05 |
| [8.3.0](#830) | 2018-10-08 |
| [8.2.0](#820) | 2018-08-29 |
| [8.1.0](#810) | 2018-04-30 |
| [8.0.0](#800) | 2018-03-12 |
| [7.7.0](#770) | 2018-01-10 |
| [7.6.0](#760) | 2017-10-10 |
| [7.5.0](#750) | 2017-08-29 |
| [7.4.0](#740) | 2017-07-05 |
| [7.3.1](#731) | 2017-06-27 |
| [7.3.0](#730) | 2017-06-26 |
| [7.2.0](#720) | 2017-04-19 |
| [7.1.0](#710) | 2017-03-23 |
| [7.0.0](#700) | 2017-02-21 |
| [6.9.0](#690) | 2017-01-12 |
| [6.8.0](#680) | 2016-11-09 |
| [6.7.0](#670) | 2016-09-21 |
| [6.6.1](#661) | 2016-08-25 |
| [6.6.0](#660) | 2016-08-23 |
| [6.5.0](#650) | 2016-06-28 |
| [6.4.0](#640) | 2016-06-14 |
| [6.3.0](#630) | 2016-05-18 |
| [6.2.1](#621) | 2016-05-02 |
| [6.2.0](#620) | 2016-04-12 |
| [6.1.0](#610) | 2016-03-17 |
| [6.0.1](#601) | 2016-03-09 |
| [6.0.0](#600) | 2016-03-07 |
| [5.5.0](#550) | 2016-02-18 |
| [5.4.0](#540) | 2016-01-26 |
| [5.3.0](#530) | 2016-01-19 |
| [5.2.0](#520) | 2015-12-15 |
| [5.1.1](#511) | 2015-11-17 |
| [5.1.0](#510) | 2015-11-09 |
| [5.0.0](#500) | 2015-10-26 |
| [4.23.4](#4234) | 2015-07-23 |
| [4.23.3](#4233) | 2015-07-17 |
| [4.23.2](#4232) | 2015-07-15 |
| [4.23.1](#4231) | 2015-07-10 |
| [4.23.0](#4230) | 2015-07-08 |
| [4.22.0](#4220) | 2015-06-03 |
| [4.21.0](#4210) | 2015-04-30 |
| [4.20.2](#4202) | 2015-04-13 |
| [4.20.1](#4201) | 2015-04-02 |
| [4.20.0](#4200) | 2015-04-01 |
| [4.19.0](#4190) | 2015-03-04 |
| [4.18.0](#4180) | 2015-02-17 |
| [4.17.0.83](#417083) | 2015-01-02 |
| [4.17.0](#4170) | 2014-12-19 |
| [4.16.0](#4160) | N/A |
| [4.15.0](#4150) | 2014-10-28 |
| [4.14.0](#4140) | 2014-09-24 |
| [4.13.1](#4131) | 2014-09-11 |
| [4.13.0](#4130) | 2014-09-10 |
| [4.12.0](#4120) | 2014-08-28 |
| [4.11.1](#4111) | 2014-08-25 |
| [4.11.0](#4110) | 2014-07-31 |
| [4.10.1](#4101) | 2014-06-30 |
| [4.10.0](#4100) | 2014-06-30 |
| [4.9.0](#490) | 2014-05-20 |
| [4.8.2](#482) | 2014-06-19 |
| [4.8.1](#481) | 2014-05-27 |
| [4.8.0.47](#48047) | 2014-04-24 |
| [4.7.5.43](#47543) | 2014-04-01 |
| [4.6.5.40](#46540) | 2014-02-27 |
| [4.5.5.38](#45538) | 2014-02-13 |
| [4.4.5.35](#44535) | 2014-01-08 |
| [4.3.5.33](#43533) | 2013-12-11 |

## 8.6 ##

### End of Life Notices ###

### New Features ###

* Requests for Laravel's built-in automatic handling of CORS HTTP OPTIONS requests
  will now be given the transaction name `_CORS_HTTP`

### Upgrade Notices ###

### Notes ###

### Bug Fixes ###

* A potential segfault when using PHP 7.3, opcache and multiple PHP workers has
  been fixed.

* Uncaught exceptions within a job being executed by a Laravel Queue worker are
  now reported correctly.

* Invoking [`function_exists()`](https://secure.php.net/function_exists) on a
  function disabled with the
  [`disable_functions`](https://secure.php.net/manual/en/ini.core.php#ini.disable-functions)
  configuration directive will now correctly return `false`.

### Internal Changes ###

### Acquia-only Notes ###

## 8.5 ##

### End of Life Notices ###

### New Features ###

* Added support for PHP 7.3.
* Http Span Events in Distributed Tracing now include the "http.method".
* We expanded [PHPUnit support](https://blog.newrelic.com/product-news/create-phpunit-dashboard/) to include PHPUnit 6, PHPUnit 7, and PHPUnit 8.

### Upgrade Notices ###

### Notes ###

### Bug Fixes ###

### Internal Changes ###

### Acquia-only Notes ###

## 8.4 ##

### End of Life Notices ###

### New Features ###

**Support for Distributed tracing**

Distributed tracing lets you see the path that a request
takes as it travels through your distributed system. By
showing the distributed activity through a unified view,
you can troubleshoot and understand a complex system better
than ever before.

Distributed tracing is available with an APM Pro or
equivalent subscription. To see a complete distributed
trace, you need to enable the feature on a set of
neighboring services. Enabling distributed tracing changes
the behavior of some New Relic features, so carefully
consult the transition guide before you enable this feature.

To enable distributed tracing, two parameters should be changed
in the newrelic.ini file:

    newrelic.distributed_tracing_enabled = true
and

    newrelic.transaction_tracer.threshold = 0

### Upgrade Notices ###

### Notes ###

### Bug Fixes ###

* A bug in the PHP agent resulted in `databaseCallCount` attributes no longer being attached to `Transaction`
  events. These attributes have been restored.

* Predis 2 cluster connections could not be instrumented due to internal
  changes in Predis. This has been fixed.

### Internal Changes ###

* The user agent reported by the daemon now includes the agent language and
  version in the general format (where `LANG` is the language, `X.Y.Z` is the
  agent version, and `A.B.C` is the daemon version):
  `NewRelic-LANG-Agent/X.Y.Z NewRelic-GoDaemon/A.B.C`

### Acquia-only Notes ###

## 8.3.0 ##

### Bug Fixes ###

* In rare cases, during a reset of the apache web server, the agent and
  `mod_php` would hang with 100% CPU utilization. This has been fixed.

### Internal Changes ###

* Includes BetterCAT v0.2, which is not yet customer facing.

## 8.2.0 ##

### Bug Fixes ###

* Fixed bug where a Drupal hook whose hook name matched its module name resulted in the generation of an empty metric name.

### Internal Changes ###

* Axiom now offers `nr_strempty()` as a safe check for strings that that are "".

* Updated the vendored flatbuffers library for Go to version 1.9.0.

* New fields have been added to the `APPINFO` response message to transmit
  harvest timing information. This does not break backward or forward
  compatibility: old agents will ignore the fields, and old daemons simply
  won't send the fields.

* Includes in progress version of BetterCAT v0.1, which is not yet customer facing

## 8.1.0 ##

### Upgrade Notices ###

* The PHP Agent API call [`newrelic_set_appname`](https://docs.newrelic.com/docs/agents/php-agent/php-agent-api/newrelic_set_appname)
  has been updated with security improvements in anticipation of future releases.

### Internal Changes ###

* When a LASP policy is set with `newrelic.security_policies_token`, the
  license key cannot be changed using `newrelic_set_appname()`. Attempting
  to do so will result in a PHP warning.

## 8.0.0 ##

### Upgrade Notices ###

* `newrelic.daemon.ssl` ini setting has been removed to increase security. TLS will
   now always be used in communication with New Relic Servers.

* Laravel Queue support has now been enabled for all users.

  If `newrelic.feature_flag=laravel_queue` is set, it will now be ignored.

### Bug Fixes ###

* On FreeBSD and Solaris, when
  [`newrelic.daemon.port`](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-configuration#inivar-daemon-port)
  is configured to use TCP to connect the agent to the daemon, the agent would
  have difficulty receiving application configuration data from the daemon due
  to the default timeout of 100 milliseconds matching the delay enforced by the
  FreeBSD kernel as part of its implementation of
  [Nagle's algorithm](https://en.wikipedia.org/wiki/Nagle%27s_algorithm).

  The PHP agent will now set the `TCP_NODELAY` flag when connecting to the
  daemon via TCP, which increases the reliability of the agent-daemon
  connection.

### Internal Changes ###

* External node creation was refactored to use a unified API within axiom. This
  should be invisible to customers, but any unusual external segment reporting
  issues that began in this version should likely be investigated in
  conjunction with those changes.

* Datastore node creation was slightly modified to make it easier to reuse in
  the C agent.

* Laravel Queue support can be disabled via the private setting
  `newrelic.special=disable_laravel_queue`.


## 7.7.0 ##

### New Features ###

* Support for PHP 7.2 has been added.

### Bug Fixes ###

* Datastore metrics would not be generated for Predis users who stopped and
  started the transaction after instantiating a `Predis\Client` object. (This
  included users using the `newrelic_set_appname()` API.)

  This has been fixed.

* Laravel queueing support stopped working in Laravel 5.5 due to a Laravel API change.
  Laravel queuing support has been fixed for Laravel versions 5.5+.

### Internal Changes ###

* Users with feature flags set to allow for fast harvest cycles could
  experience daemon panics if an application timed out. This has been fixed.

* The harvest whitelist has been removed.  Users who were previously on
  the whitelist can now request customization of harvest cycle report periods
  using feature flags.

* Updated Docker container ID-matching rules to align with latest metadata specification.

## 7.6.0 ##

### New Features ###

* Internal functions can now be instrumented by enabling the
  `newrelic.transaction_tracer.internal_functions_enabled` configuration
  setting. When enabled, internal functions will appear in transaction traces
  like PHP functions.

  Note that enabling this option may result in transactions being slower,
  especially when collecting many traces from PHP 5.x. Enabling this option is
  only recommended when specifically debugging performance issues where an
  internal function is suspected to be slow.

### Bug Fixes ###

* In some cases, Magento 2's UI Component JSON data sources return with an (incorrect)
  `Content-Type` of `text/html`. This resulted in auto instrumentation insertions into
  JSON data that contained a `<head>` string. We now identify these specific JSON data
  sources and ensure the auto instrumentation is not inserted into this JSON data.

* On PHP 7, using `file_get_contents()` to get a HTTP URL in a function which
  also read from `$http_response_header` resulted in the external call not
  being linked correctly in the APM UI: the external call would appear in
  transaction traces without a link to the receiving application. This has been
  fixed.

* Using [`curl_setopt_array()`](http://php.net/curl_setopt_array) to set
  options on a cURL handle could result in cross application tracing failing.
  This has been fixed.

* When generating explain plans for queries issued with MySQLi, the PHP agent
  could bind parameters from the wrong query if the PHP object ID had been
  reused internally. This has been fixed.

* Due to a change in Laravel 5.5's router API, the PHP agent would crash when
  automatically naming a Laravel transaction. This has been fixed.

* On PHP 7, when naming transactions for the Laravel 5 framework, well-formed
  transaction names were being replaced by the too-generic `$METHOD/index.php`.
  This has been fixed.

* Invoking `newrelic_record_datastore_segment()` before the PHP agent had
  connected to New Relic would result in the query callback silently failing to
  execute. This has been fixed; the callback will always be invoked provided
  parameters of the correct type are given to the function, otherwise an
  `E_WARNING` will be generated.

## 7.5.0 ##

### New Features ###

* A new API function for timing datastore functions that the PHP agent doesn't
  natively instrument has been added: `newrelic_record_datastore_segment()`.
  Documentation and usage examples can be found on the
  [New Relic Documentation](https://docs.newrelic.com/docs/agents/php-agent/php-agent-api/newrelic_record_datastore_segment)
  site.

* The agent now collects additional metadata when running within AWS, GCP, Azure,
  and PCF. This information is used to provide an enhanced experience when the
  agent is deployed on those platforms.

### Bug Fixes ###

* When generating explain plans for queries issued with MySQLi, the PHP agent
  could bind parameters from the wrong query if the PHP object ID had been
  reused internally. This has been fixed.

## 7.4.0 ##

### Bug Fixes ###

* On Laravel 5.4, previous versions of the PHP agent named transactions handled
  by routes without explicit names based on the request method and front
  controller name, instead of using the route URI as with other versions of
  Laravel. This has been fixed, and automatic transaction names are once again
  consistent across all Laravel versions.

* On the first request served by a PHP process, a Drupal 6 or 7 hook that was
  invoked with `module_invoke()` before it had been invoked by
  `module_invoke_all()` could have its module name reported as an empty string.
  This has been fixed.

* On PHP 7.0 and 7.1, a Drupal 6 or 7 hook invoked via `module_invoke_all()`
  could report the module name as an empty string in some situations. This has
  been fixed.

## 7.3.1 ##

### Bug Fixes ###

* Users of Guzzle 4 or 5 may have seen crashes in version 7.3.0 of the PHP
  agent due to a bug in the agent's request shutdown instrumentation for that
  library. This has been fixed.

## 7.3.0 ##

### Upgrade Notices ###

* Laravel Queue instrumentation has been updated to better interoperate with
  other New Relic APM features. It is possible that messages sent from PHP
  installs running version 7.3 (or later) of the PHP agent may not be correctly
  linked to the message queue worker transaction if the worker is running with
  version 7.2 or earlier of the PHP agent.

  If you use the Laravel Queue instrumentation, we recommend ensuring that all
  agents used to produce and consume messages are upgraded to version 7.3
  simultaneously to avoid unexpected service map issues.

### Bug Fixes ###

* Compatibility with extensions that install opcode handlers on PHP 7 has been
  improved, most notably with uopz. While we still do not recommend using other
  extensions that hook deeply into PHP (such as uopz, runkit, or xdebug) with
  the New Relic PHP agent, this change should result in improved functionality.

* A potential segfault when using Predis in a session handler invoked at
  request shutdown has been fixed.

* External requests issued using Guzzle 6 are now properly instrumented when
  the `GuzzleHttp\Client` class has been extended.

### Internal Changes ###

* If the Guzzle 6 middleware is not callable, we will now prevent the error
  message from bubbling to the user and send a supportability metric.

## 7.2.0 ##

### Upgrade Notices ###

* Users of the `newrelic-php5` Debian package with PHP 5.2: due to discontinued
  support of PHP 5.2, upgrading to version 7.x of the PHP agent may result in
  your applications no longer reporting to New Relic. We advise that you upgrade
  to a supported version of PHP.

  If upgrading PHP is not possible, you can use `apt-get` and `dpkg` to install
  version 6.9 of the PHP agent and then hold that installed version by running
  these commands as root:

    ```sh
    apt-get update
    apt-get upgrade newrelic-php5-common=6.9.0.182 newrelic-daemon=6.9.0.182 newrelic-php5=6.9.0.182
    echo 'newrelic-php5-common hold' | dpkg --set-selections
    ```

### Bug Fixes ###

* Automatic transaction naming has been fixed for Symfony 1 users who replaced
  the controller class via `factories.yml` with one that did not call
  `sfFrontWebController::dispatch()` or `sfController::forward()`.

* Datastore metrics could not be generated for Predis 0.7 and 0.8 in version
  7.1 of the PHP agent. This has been fixed.

* The agent would not report `Zend_Http_Client` external calls when the Zend
  Framework was used directly as a library or indirectly by a framework like
  Symfony or Magento. This has been fixed.

### Internal Changes ###

* Support for PHP 5.2 has been completely removed from our code base, rather
  than just being disabled at build time.

## 7.1.0 ##

### New Features ###

* Added support for the MongoDB library

  The PHP agent now supports the MongoDB userland library, which is the
  recommended interface to the mongodb extension. Users of this library will see
  new metrics in the Databases tab, nodes in transaction traces, and breakdown
  metrics on individual transaction pages.

* Database instance information available for Redis, Predis, and MongoDB

  Queries and commands issued using the PECL Redis extension, the Predis
  library, and the MongoDB library now capture instance information. Transaction
  traces now include the name of the host (including the port or socket) and
  database that the query was run against.

  Collection of host information can be disabled by setting
  `newrelic.datastore_tracer.instance_reporting.enabled = false` in the
  `newrelic.ini`. Similarly, the database name can be omitted with
  `newrelic.datastore_tracer.database_name_reporting.enabled = false`.

### Upgrade Notices ###

* Commands in Predis pipelines are now reported individually, rather than being
  rolled into a single `pipeline` query.

### Bug Fixes ###

* When using PHP 7.0 or 7.1, calling `curl_setopt()` with the
  `CURLOPT_HTTPHEADER` option and an array with elements that have been
  accessed by reference (for example, with `foreach ($array as $k => &$v)`)
  would result in the headers used by New Relic to track external calls
  overwriting other custom headers. This has been fixed.

### Internal Changes ###

* Metric rules that matched but did not change the metric name would not
  terminate rule processing even if the metric rule had the terminate flag set.
  This has been fixed.

## 7.0.0 ##

### End of Life Notices ###

* The New Relic PHP agent no longer supports PHP 5.2. New Relic highly
  encourages upgrading to a supported version of PHP - 5.6 or higher. If you
  would like to continue running the New Relic PHP agent with PHP 5.2, we
  recommend using version 6.9 of the agent, however please note that we can only
  offer limited support in this case.

### New Features ###

* Automatic transaction naming for Magento 2 has been improved. Previously,
  only requests that were handled by Magento's normal routing were
  automatically named, which resulted in request types that utilised Magento's
  [interceptor/plugin](http://devdocs.magento.com/guides/v2.0/extension-dev-guide/plugins.html)
  functionality being named as `unknown`. This primarily affected users of the
  REST or SOAP web services.

  REST and SOAP requests will now be named in the `Webapi/Rest` and
  `Webapi/Soap` namespaces, respectively, and will be named according to the
  service class and method that handle the request.

  Third party modules that add an interceptor on
  `Magento\Framework\App\FrontControllerInterface` to override the `dispatch()`
  method will result in transaction names that are based on the interceptor
  class name. We suggest using the
  [`newrelic_name_transaction()`](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-api#api-name-wt)
  API function from within the module to name the transaction if more precision
  is required.

### Internal Changes ###

* Add `newrelic.special.appinfo_timeout` to adjust the timeout when the agent
  is waiting for an `APPINFO` response.

## 6.9.0 ##

### New Features ###

* Instance information for the pgsql and PDO pgsql extensions is now captured.
  Slow database queries and transaction traces now include the name of the host
  (including the port or socket) and database that the query was run against.

  Collection of host information can be disabled by setting
  `newrelic.datastore_tracer.instance_reporting.enabled = false` in the
  `newrelic.ini`. Similarly, the database name can be omitted with
  `newrelic.datastore_tracer.database_name_reporting.enabled = false`.

### Internal Changes ###

* Add account 990212 to the whitelist for extra event harvests.

## 6.8.0 ##

### End of Life Notices ###

* Support for PHP 5.2 in the New Relic PHP agent is deprecated and will be
  removed in a future version. New Relic highly encourages upgrading to a
  supported version of PHP - 5.6 or higher. If you would like to continue
  running the New Relic PHP agent with PHP 5.2, we recommend using version 6.8
  of the agent, however please note that we can only offer limited support in
  this case.

### New Features ###

* Support for PHP 7.1 has been added.

* The elapsed time after which an application will be considered inactive
  can now be configured using the `newrelic.daemon.app_timeout` setting. This
  setting is useful when the only transactions being executed are very
  infrequent, such background processing using a cron job. Previously, an
  application was considering inactive after ten minutes with no transactions.

* Instance information for the mysql, mysqli, and PDO mysql extensions is now
  captured. Slow database queries and transaction traces now include the name of
  the host (including the port or socket) and database that the query was run
  against.

  Collection of host information can be disabled by setting
  `newrelic.datastore_tracer.instance_reporting.enabled = false` in the
  `newrelic.ini`. Similarly, the database name can be omitted with
  `newrelic.datastore_tracer.database_name_reporting.enabled = false`.

### Bug Fixes ###

* Automatic transaction naming for MediaWiki versions 1.18 and later was broken
  in version 6.3 of the New Relic PHP agent. This has been fixed.

## 6.7.0 ##

### New Features ###

* PHPUnit test events

  Support has been added for the popular unit test library PHPUnit (versions 3.7
  to 5.5, inclusive). When enabled, the agent reports test results as
  custom Insights events. Test suite summary data are sent as "TestSuite"
  events, while individual test cases are sent as "Test" events.

  You can enable this feature in your newrelic.ini file with
  `newrelic.phpunit_events.enabled = true`. Depending on your events retention
  policy, enabling this setting may impact your New Relic billing statement.

  Read more about this feature [on our documentation
  site](https://docs.newrelic.com/docs/agents/php-agent/frameworks-libraries/analyze-phpunit-test-data-insights).

* Support has been added for Slim Framework 2.x and 3.x.

### Upgrade Notices ###

* To avoid potential deadlocks, MySQL SELECT queries that use the
  [`FOR UPDATE` or `LOCK IN SHARE MODE`](http://dev.mysql.com/doc/refman/5.7/en/innodb-locking-reads.html)
  suffixes will no longer generate explain plans.

### Bug Fixes ###

* Automatic page load timing instrumentation was broken on PHP 5.4-7.0 in
  certain edge cases: most notably, when the `<head>` element was not in the
  first 40kB of the page content, or when the `</body>` element was not in the
  last few kilobytes of content. This has been fixed.

* Prevent real user monitoring JavaScript injection when the Content-Type
  is first set to text/html then subsequently changed to something else.
  e.g. application/json. This was a regression introduced in version 6.2.

### Internal Changes ###

* The daemon is now built with Go 1.7.

## 6.6.1 ##

### Bug Fixes ###

* When using version 6.6.0 of the PHP agent, instantiating a PDO object with
  options set could result in PHP crashing at the end of handling a request.
  This has been fixed.

## 6.6.0 ##

### New Features ###

* Support has been added for Laravel 5.3.

* Experimental Laravel Queue support.

  Experimental support for the
  [Laravel Queue component](http://laravel.com/docs/5.2/queues) has been added.
  This supports all versions of Laravel from 4.0 to 5.3, inclusive.

  Whether run via `artisan queue:listen` or `artisan queue:work`, queue workers
  will now generate a separate background transaction for each job that is
  processed. These transactions will be named with the class name of the job,
  or `IlluminateQueueClosure` if a closure was queued.

  Additionally, a transaction that queues one or more jobs will be linked to
  the background transaction(s) that processed those jobs on the transaction
  map in the New Relic UI.

  To enable this support, you will need to set the `laravel_queue` feature flag
  in newrelic.ini. For most users, this will simply involve adding this line:

      newrelic.feature_flag=laravel_queue

  As this is presently an experimental feature, we would welcome feedback — both
  positive and negative — on whether this feature helps users using the Laravel
  Queue component better visualise the structure and performance of their
  applications.

  All going well, it is anticipated that Laravel Queue support will be enabled
  by default in a future version of the agent.

### Upgrade Notices ###

* Symfony 1 transactions that generate 404 errors due to no routes being
  matched will now be named based on the module and action that are actually
  used, rather than the module and action inferred from the request URL. In
  most cases, this will result in the transaction name being based on the
  `error_404_module` and `error_404_action` Symfony settings.

  Note that, as a result of this change, if the configured 404 action calls
  `sfAction::forward()` to forward to another action, the transaction name will
  be based on the final action. All other uses of `sfAction::forward()` are
  unaffected.

### Bug Fixes ###

* Previously, calling `PDO::query` with more than one argument resulted in
  the query being counted under the 'other' category on the Datastores page
  in the New Relic UI. This has been fixed. The agent will now correctly
  categorize the query as 'select', 'insert', 'update', or 'delete'.

* Previously, calling `PDO::query` with more than one argument resulted in
  the agent reporting "(unknown sql)" if the query was slow. (i.e. It exceeded
  the threshold given by the
  [newrelic.transaction\_tracer.explain\_threshold](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-configuration#inivar-tt-epthreshold)
  setting.) This has been fixed.

* Previously, calling `SQLite3::querySingle` with two arguments resulted in
  the query being counted under the 'other' category on the Datastores page
  in the New Relic UI. This has been fixed. The agent will now correctly
  categorize the query as 'select', 'insert', 'update', or 'delete'.

* Previously, calling `SQLite3::querySingle` with two arguments resulted in
  the agent reporting "(unknown sql)" if the query was slow. (i.e. It exceeded
  the threshold given by the
  [newrelic.transaction\_tracer.explain\_threshold](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-configuration#inivar-tt-epthreshold)
  setting.) This has been fixed.

* Transient Magento 2 tables now have the unique numbers removed from their
  names to avoid metric grouping issues.

* Pressflow 6 applications were incorrectly detected as Drupal 7 applications.
  This could cause POST requests made using `drupal_http_request` to be
  converted into GET requests. This has been fixed.

* Explain plan generation is now always performed on a separate database
  connection. This has been changed to avoid issues when using MySQL's
  `SELECT FOUND_ROWS()` feature.

* Previously, instrumenting method calls that resolved to `__call()` or
  `__callStatic()` calls could result in crashes on PHP 7. This primarily
  manifested in crashes for users of certain WordPress plugins (for example,
  wp-external-links) that registered hooks or actions that used magic methods.
  This has been fixed.

## 6.5.0 ##

### New Features ###

* Support has been added for Zend Framework 3.

### Bug Fixes ###

* The behaviour of the installer when attempting to detect PHP installations has
  been tightened up. Previously, a directory structure such as
  `/usr/local/php/php/*` would be detected as an installation of PHP in
  `/usr/local/php`, as the nested `php` directory would be wrongly assumed to be
  a PHP CLI binary. The installer now checks if the nested `php` (or
  `php-config`) is a directory, and ignores the path if so.

  Although this is likely to improve the installer in multiple environments,
  most noticeably, this fix removes erroneous messages about there being
  multiple PHP installations when installing the PHP agent within the
  [Docker Hub's official PHP images](https://hub.docker.com/_/php).

* Spurious messages like `ALERT: oops, unknown child` could appear in the PHP
  FPM log when a New Relic daemon process was started. This has been fixed.

### Internal Changes ###

* The agent will now attempt to log a PHP stack trace in addition to the C
  stack trace that is currently captured. This may provide important context
  when triaging crashes.

* A new special logging mode has been added to help debugging CAT issues.
  Setting `newrelic.special = debug_cat` in the ini file will cause the agent
  to log all inbound and outbound CAT headers. Note, the headers are logged as
  they would appear on the wire. i.e. They remaining obfuscated. This preserves
  as much information as possible.

## 6.4.0 ##

### New Features ###

* Ignore Errors

  A new `newrelic.error_collector.ignore_errors` setting has been added. When
  configured, this setting can be used to prevent errors, such as `E_WARNING`,
  from being reported to New Relic. This setting does not affect errors reported
  via the `newrelic_notice_error` API.

* Ignore Exceptions

  A new `newrelic.error_collector.ignore_exceptions` setting has been added.
  This setting accepts a comma-separated list of fully-qualified class names.
  When an uncaught exception occurs, the agent will not report the exception
  to New Relic if it is an instance of a class in this list. This setting does
  not affect errors reported via the `newrelic_notice_error` API.

### Upgrade Notices ###

* Guzzle support is now enabled by default. It can be disabled by setting the
  new `newrelic.guzzle.enabled` setting to false.

  The `guzzle` feature flag that was previously used to enable Guzzle support
  is now ignored.

### Bug Fixes ###

* The first request in each PHP process would remove any output handlers
  installed using the `output_handler` configuration setting for users of PHP
  5.4 and later. This has been fixed: existing output handlers will now remain
  untouched.

* Transient WordPress hooks now have the unique numbers removed from their names
  to avoid metric grouping issues.

* In some cases, WordPress plugin metrics were not reported on PHP 5.2 to 5.4,
  inclusive. This has been fixed.

* A regression in version 6.3 could cause Guzzle 6 users to lose support for
  cross application tracing and service maps, particularly when Guzzle 6 was
  autoloaded as a result of instantiating the only `GuzzleHttp\Client` object
  used within the lifetime of the script. This has been fixed.

### Internal Changes ###

* Added the utilization configuration settings introduced in the second
  utilization spec version, and incremented the utilization version number.

## 6.3.0 ##

### New Features ###

* Support for Postgres prepared statements has been added when the
  `pg_prepare` and `pg_execute` functions are used. The agent will
  now report Datastore metrics as well as capture slow queries.

### Bug Fixes ###

* Added a workaround for a stack corruption bug in very old versions of the
  Suhosin extension when used with PHP 5.3 and newer. The bug exists in Suhosin
  0.9.29 and older only. Newer versions are not affected.

* Automatic transaction naming could fail in certain circumstances on Laravel
  4.1 and later on PHP 5 when a before route filter was installed. This has
  been fixed.

* Automatic transaction naming could fail on Laravel 5.x when a global
  middleware handled the request without forwarding it deeper into the
  application. For example, the default Authenticate middleware returns a
  `401 Forbidden` response to unauthenticated requests, preventing proper
  transaction naming. This has been fixed.

* A segfault could occur in memory constrained environments, particularly on
  PHP 7. This has been fixed.

### Internal Changes ###

* We converted all user function wrappers to exec wrappers to ensure a
  consistent style that is more easily reviewed.

## 6.2.1 ##

### New Features ###

* We're proud to announce initial support for Alpine Linux 3.1 and newer.
  Currently, [manual installation](https://docs.newrelic.com/docs/agents/php-agent/installation/php-agent-installation-tar-file) is required.

## 6.2.0 ##

### New Features ###

* The PHP agent now has support for Doctrine 2 DQL Queries. When running a DQL
  query, it will appear alongside the SQL in both Slow SQLs and Transaction
  Traces.

### Notes ###

* The agent now includes attributes for HTTP response headers in web
  transactions:
    * `response.headers.contentType`
    * `response.headers.contentLength`

### Bug Fixes ###

* newrelic.transaction_events.enabled wasn't being observed.  This has been
  fixed.

* The PHP in SUSE and OpenSUSE Linux was incompatible with some of the agent's
  features, including RUM and CAT. This issue has been fixed.

## 6.1.0 ##

### New Features ###

* Support has been added for Magento 2.

### Bug Fixes ###

* Support for Laravel 5.2 has been added. Previously, Laravel 5.2 applications
  would fail to start with a `BadMethodCallException`. This was due to the
  agent's attempted use of a route filter for automatic transaction naming.
  Automatic transaction naming for Laravel 5.2 no longer relies on a route
  filter preventing the error. Automatic transaction naming for older versions
  of Laravel is unaffected.

* Calling `phpversion("newrelic")` could return `"unreleased"` instead of
  the actual version number. In turn, this caused `composer show --platform`
  to report a version of 0. This has been fixed. The version number reported
  by `phpinfo()` was not affected by this bug.

* Fixed a potential segfault when using `Zend_Http_Client` under PHP 5.5 or 5.6
  with opcache enabled.

## 6.0.1 ##

### Bug Fixes ###

* Fixed a bug that could cause segfaults on PHP 7.0 when a `__call()` method
  was invoked and was slow enough to generate a trace node.

## 6.0.0 ##

### New Features ###

* Support has been added for PHP 7.0.

## 5.5.0 ##

### New Features ###

* The agent will now gather more information about the host system, including
  whether the system is running on Docker or AWS. This will be used to support
  upcoming features.

### Notes ###

* We've changed the timestamp source for error events, which drive the Error
  Analytics page. Previously, the event's timestamp was the transaction's start
  time. It is now the time when the error occurred.

## 5.4.0 ##

### New Features ###

* Experimental support for instrumenting Guzzle has been added.

  We are very pleased to announce that we've added experimental support for
  instrumenting Guzzle 3, 4, 5, and 6. Requests made using Guzzle will now show
  up as externals in the same way as requests made using other instrumented
  libraries, like curl, `drupal_http_request()` and `file_get_contents()`.

  To enable the experimental support, you will need to add a feature flag to
  your newrelic.ini: `newrelic.feature_flag = guzzle`. NOTE: We intend to enable
  the Guzzle support by default in a future release.

  More information can be found [on our Guzzle documentation
  page](https://docs.newrelic.com/docs/agents/php-agent/frameworks-
  libraries/guzzle).

## 5.3.0 ##

### New Features ###

* WordPress tab in the New Relic UI

  We're pleased to announce a new UI tab just for WordPress-specific data!
  WordPress applications now show information about their hooks, plugins, and
  themes.

  For more details, [see our documentation](https://docs.newrelic.com/docs/agents/php-agent/frameworks-libraries/wordpress-specific-functionality).

### Internal Changes ###

* We removed the Supportability/RPM/Drupal metric, as it no longer drives the
  framework-specific tab in the UI. Instead, the UI uses an application's
  Environment Attributes, which the Collector creates based on
  Supportability/framework/*/(forced|detected) metrics.

* We now create the `Supportability/framework/*` metric at the end of each
  transaction, rather than just when it is detected.

* We added preliminary support for Guzzle 3, 4, 5, and 6. The instrumentation is
  hidden behind `newrelic.feature_flag = guzzle`.

## 5.2.0 ##

### New Features ###

* On Linux, abstract sockets are now supported for agent to daemon communication.
  An abstract socket can be created by prefixing the value of the port setting in
  your newrelic.ini file (e.g. `newrelic.daemon.port = "@newrelic-daemon"`) or
  newrelic.cfg file (e.g. `port = "@newrelic-daemon"`).

* The daemon can now be passed a `--define <setting>=<value>` flag that allows
  setting arbitrary parameters from your newrelic.cfg file. These parameters
  will take precedence over settings from that file.

* Drupal 8 requests that are successful full page cache hits for anonymous
  users (handled by Drupal\page_cache\StackMiddleware\PageCache) are now
  given the name WebTransaction/Action/page_cache.

* Support for automatically naming transactions has been added for Symfony 2.8
  and 3.0.

* Support for automatically naming transactions has been added for MediaWiki
  1.18.0 and newer.

### Upgrade Notices ###

* It is now possible to disable reporting of errors for exceptions that are
  handled by an exception handler installed with `set_exception_handler()` by
  enabling the `newrelic.error_collector.ignore_user_exception_handler`
  setting.

* Calls to newrelic_notice_error with improper parameters will no longer record
  an error with the message "?".  Instead, no error will be recorded.
  newrelic_notice_error will now return true if an error is saved, null if the
  arguments are invalid, and false otherwise.  If newrelic_notice_error is
  called with two parameters and the second parameter is not a valid exception,
  no error will be recorded:  The first parameter is now entirely unused.

### Bug Fixes ###

* On Redhat Enterprise Linux 7 and related systems such as CentOS and Fedora,
  the `systemctl start|stop newrelic-daemon` commands were not working due
  to a bug in the newrelic-daemon init script when executed by systemd. This
  has been fixed.

* On some Linux systems, an SELinux policy could prevent the newrelic-daemon
  from determining the correct hostname. Affected systems were unable to report
  data because a valid hostname is required by New Relic. This has been fixed.
  This issue could also prevent the newrelic-daemon from determining the correct
  hostname when running in a chrooted environment, or any environment in which
  access to the /proc filesystem is limited.

* Detection of Zend Framework 2.5 and later was unreliable when the framework
  was installed via Composer. This has been fixed.

* The agent now sends the attribute `request.headers.contentLength` as an
  integer rather than a string.

* Detection of Laravel 5.1 and later was unreliable when artisan optimize was
  used. This has been fixed.

### Internal Changes ###

* A systemd unit file has been added to the tarball distribution in the scripts
  directory.

* Exceptions will not be saved in our error handler when our exception handler
  has been disabled.

* newrelic_set_appname now attempts to begin a new transaction even when there
  is no current transaction.  newrelic_set_appname will no longer fail if
  a RUM footer has been created.

## 5.1.1 ##

### New Features ###

* With this release, the agent reports
  [TransactionError](https://docs.newrelic.com/docs/insights/new-relic-insights/decorating-events/error-event-default-attributes-insights)
  events. These new events power the beta feature
  [Advanced Analytics for APM Errors](https://docs.newrelic.com/docs/apm/applications-menu/events/view-apm-errors-error-traces)
  (apply [here](https://discuss.newrelic.com/t/join-the-apm-errors-beta-of-real-time-analytics/31123)
  to participate). The error events are also available today through
  [New Relic Insights](http://newrelic.com/insights).

## 5.1.0 ##

### End of Life Notices ###

### New Features ###

* To easily distinguish dynamically assigned hosts, users can now assign a
  display name to a host with the setting `newrelic.process_host.display_name`.
  For more details, [see our docs page](https://docs.newrelic.com/docs/apm/new-relic-apm/maintenance/add-rename-remove-hosts#display_name).

* The New Relic PHP Agent now supports the latest Production Release of 64-bit
  [FreeBSD](https://www.freebsd.org/releases/), which is currently at
  10.2-RELEASE.

### Upgrade Notices ###

* Transactions run under PHP's CLI Web server will now report as Web
  transactions instead of background transactions. The
  [`newrelic_background_job()`](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-api#api-bg)
  API function can be used to manually mark the transaction as a background job
  if the old behaviour is desired.

### Notes ###

* The agent now collects the following information in web transactions (not yet
  shown in the New Relic UI): Accept, Host, Content-Length, and Content-Type
  HTTP request headers, and HTTP request method.

## 5.0.0 ##

### New Features ###
* Users can now delete applications from the UI after 20 minutes of inactivity.
  Previously, this required a restart of the New Relic PHP daemon.

### Upgrade Notices ###
* We rearchitected the daemon to enable future improvements.

* Daemon log levels are now simpler. While "verbose" and "verbosedebug" will
  still work, "debug" now gives you all the debugging information. Agent log
  levels remain unchanged.

### Bug Fixes ###

### End of Life Notices ###
* For the few customers who start the daemon manually, the newly renamed
  command-line flags can be found with the **-h** or **--help** flags. The
  following old flags will be removed in a future release:
  - [-p pidfile]
  - [-d level]
  - [-c config]
  - [-l logfile]
  - [-P port]
  - [-s]
  - [-n]
  - [-b SSL-certificate-bundle]
  - [-S SSL-certificate-path]
  - [-H host]
  - [-x proxy]
  - [-a auditlog]

### Internal Changes ###

* Add account 261952 to the whitelist for extra event harvests.

## 4.23.4 ##

### Bug Fixes ###

* A memory leak that could occur when a large number of PDO or mysqli
  statements were prepared and executed has been fixed.

## 4.23.3 ##

### Bug Fixes ###

* This release fixes a rare bug where applications that had the ionCube loader
  enabled and called `set_exception_handler()` repeatedly could crash.

## 4.23.2 ##

### New Features ###

* Exceptions that are ignored by Laravel 5's default exception handler will
  also now be ignored by the PHP agent's error reporting feature. This can
  be controlled by adding or removing entries in the `dontReport`
  property of Laravel's default exception handler, as described
  [in the Laravel documentation](http://laravel.com/docs/5.1/errors#report-method).

## 4.23.1 ##

### Bug Fixes ###

* Under certain circumstances, version 4.23.0 of the PHP agent could crash when
  `newrelic_set_appname()` or `newrelic_end_transaction()` were called and a
  custom exception handler had been installed via `set_exception_handler()`.
  This has been fixed.

* Under certain circumstances, Drupal applications that rely on the
  `newrelic_set_appname()` API function to start transactions would
  not see the Drupal tab in the New Relic UI. This has been fixed.

### Internal Changes ###

* Add account 261952 to the whitelist for extra event harvests.

### Acquia-only Notes ###

## 4.23.0 ##

### End of Life Notices ###

### New Features ###

* Exceptions that are uncaught and handled only by a last resort exception
  handler installed via
  [`set_exception_handler()`](http://php.net/set_exception_handler) will now
  generate errors that can be viewed in the APM UI. Previously they would not
  be sent to New Relic at all.

  As a result of this, some customers will see more errors in the APM UI than
  they had previously seen.

### Upgrade Notices ###

### Bug Fixes ###

* In certain circumstances, such as an exception being thrown during database
  connection, errors generated by uncaught exceptions could result in the stack
  trace shown in APM including function argument values, which could potentially
  expose database passwords and other sensitive information. This problem has
  been fixed.

  We now catch these formerly-uncaught exceptions and thus have control over how
  the stack trace is built, ensuring that function argument values will no
  longer show up in those stack traces.

### Internal Changes ###

* Added a hash table implementation.

## 4.22.0 ##

### New Features ###

* Added support for the Predis library.

  The PHP agent now supports the Predis library. Users of this library will see
  new metrics in the Database tab, nodes in transaction traces, and breakdown
  metrics on individual transaction pages.  Pipelining multiple commands will
  result in those commands being grouped and treated as a single command metric
  named `pipeline`.

  Note that our Predis support does not include instrumentation for commands
  wrapped in `MULTI` and `EXEC` via the Predis client's `transaction` method.
  We also do not instrument the `executeRaw` or `monitor` methods of Predis,
  nor its PubSub functionality.

* Increase specificity of PECL Redis metrics.

  Metrics made for the PECL Redis extension will now more closely match the
  name of the function called.  For example, the `redis::setex` method will
  now create a metric with the text `setex`, rather than simply `set`.

* Background transactions that are Laravel artisan commands are now named.

  Instead of being named `unknown`, Laravel artisan commands will now be named
  based on the command. For instance, the command `./artisan cache:clear`
  will result in a background transaction named `Artisan/cache:clear`.

* The log file format has been extended to include the UTC offset of the
  local timezone. This makes it easier to match log entries to time windows
  in the New Relic UI.

### Upgrade Notices ###

### Bug Fixes ###

### Internal Changes ###

* Ensure that url rules, transaction name rules, and segment term rules apply to
  Apdex metrics by using the transaction name, rather than the path, to generate
  the Apdex metric.

## 4.21.0 ##

### New Features ###

* Zend Framework 2 transaction naming support.

  Support has been added for naming transactions automatically based on ZF2
  route names. Transaction naming will work with routes set in
  `module.config.php`, or anything else using the `setMatchedRouteName` method
  of `Zend\Mvc\Router\RouteMatch` (or the same methods of the
  corresponding `HTTP` and `Console` routers).

  For example:

      'router' => array(
        'routes' => array(
          'route1' => array( ... )
            'route'    => '/foo[/:bar][/:baz]',
              'constraints' => array( ... ),
                 'defaults' => array(
      ...
          'route2' => array( ... )

  Anything the router matches to `route1` will be named `route1` regardless of
  URL, parameters or controller. 404 errors and similar will be named
  "unknown".

* Silex transaction naming support.

  Support has been added for naming transactions automatically based on Silex
  controller names. If a route name has been set using the `Controller::bind()`
  method, then that will be used, otherwise the default controller name
  generated by Silex will be used instead.

  For example:

      // This will be named with the autogenerated `GET_hello_name`.
      $app->get('/hello/{name}', function ($name) { ... });

      // This will be named `hello`, as bound.
      $app->get('/hello/{name}', function ($name) { ... })->bind('hello');

  Automatic transaction naming will work on all Silex 1.x versions provided the
  default `kernel` service has not been replaced with an object of a type other
  than `Symfony\Component\HttpKernel\HttpKernel`. If the `kernel` service has
  been replaced with something else, you will need to continue to use the
  `newrelic_name_transaction` function to name your transactions.

### Bug Fixes ###

* `databaseDuration` attributes have been restored.

  A bug in versions 4.19 and 4.20 of the PHP agent resulted in
  `databaseDuration` attributes no longer being attached to `Transaction`
  events. These have been restored, with one change: they now also include time
  spent in Memcached operations, as these are now unified with other datastores
  in New Relic APM.

* Fix application name rollup ordering issue.

  When using application rollup names, the agent had the restriction that
  any name could only appear as the first rollup name once, which prevented
  the simultaneous connection of "a;b" and "a;c".  This restriction has been
  removed.

### End of Life Notices ###

* `memcacheDuration` attributes have been removed.

  A bug in version 4.19 of the PHP agent removed support for `memcacheDuration`
  attributes in `Transaction` events. We have decided not to restore this
  functionality, as these durations are now accounted for in `databaseDuration`
  attribute values.

### Internal Changes ###

* Add "identifier" field to connect command.

  This identifier is used by the collector to look up the real agent. If an
  identifier isn't provided, the collector will create its own based on the
  first appname, which prevents a single daemon from connecting "a;b" and
  "a;c" at the same time.

  Providing the identifier below works around this issue and allows users
  more flexibility in using application rollups.

* All uses of PCRE have been abstracted into a utility library.

  This results in simplified, more consistent code in the various places in the
  agent and axiom where we use regular expressions, which are:

  * Transaction naming by file (using `newrelic.webtransaction.name.files`)
  * URL rules
  * Segment term rules
  * Auto-RUM

* Fix SSL error messages to suggest the right configuration options.

  The original messages presumably date from before version 3.0, and don't
  include the `newrelic.daemon` prefix that daemon configuration options
  generally use nowadays.

## 4.20.2 ##

### Bug Fixes ###

* Support for mixed Laravel 4 and 5 environments has been improved.

  Web servers serving both Laravel 4 and Laravel 5 applications from the same
  process pool could see autoloader errors in the Laravel 4 applications after
  the first load of a Laravel 5 page. This has been fixed, and mixed
  environments should now work as expected.

* Laravel detection has been improved.

  Starting with Laravel 5.0.15, applications with an optimized classloader use
  a new location for the optimized "compiled.php" file.  Previously, the agent
  would fail to detect Laravel in this circumstance.  The agent now looks in
  this new location as well as the old location for Laravel auto-detection.

## 4.20.1 ##

### Bug Fixes ###

* MySQLi segfaults fixed.

  Version 4.20.0 could cause the PHP agent to segfault when certain types of
  parameters were bound to `mysqli_stmt` objects. This has been fixed.

## 4.20.0 ##

### New Features ###

* Support for Laravel 5.0.

  We are pleased to announce support for the latest Laravel release: Laravel 5.
  Sites and applications using Laravel 5.0 are now automatically detected and
  named in the same way as Laravel 4 applications.

* Support for explain plans with MySQLi.

  The existing support for generating explain plans for slow SELECT queries
  with the PDO MySQL driver has been extended to MySQLi. Explain plans are controlled by the
  [newrelic.transaction_tracer.explain_enabled](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-newrelicini-settings#inivar-tt-epenabled) and
  [newrelic.transaction_tracer.explain_threshold](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-newrelicini-settings#inivar-tt-epthreshold)
  settings. These plans can be seen in the query analysis section of the
  [slow SQL trace](https://docs.newrelic.com/docs/apm/applications-menu/monitoring/database-and-slow-sql-dashboard#db_slowsql)
  dashboard in APM.

  Explain plans will be generated for queries issued through
  `mysqli_query`, `mysqli_real_query`, and `mysqli_stmt_execute`, as well as their OO
  equivalents `mysqli::query`, `mysqli::real_query`, and
  `mysqli_stmt::execute`.

  Please note that the `mysqli_multi_query` API is not currently supported,
  and that explain plans are not generated for multiple (semicolon separated) queries.

* Added Memcache connection monitoring.

  The `connect` and `pconnect` functions (and their OO counterparts) of the Memcache extension
  are now instrumented, and will generate metrics. As a result, if there is
  a problem with calling those functions, it will now be visible within New Relic.

### Bug Fixes ###

* Laravel 4 App::after() filters are no longer overridden.

  In certain circumstances, the improved Laravel transaction naming code in
  version 4.19 of the PHP agent could result in filters registered using
  Laravel 4's App::after() method no longer being executed. This has been
  fixed.

### Upgrade Notices ###

* The daemon now uses openssl 1.0.1m, rather than 1.0.1l.

* The daemon now uses curl 7.41.0 rather than 7.40.0.

### Internal Changes ###

* Remove detection and disabling of Intouch Laravel plugin.

* Add production account 24650 to flexible analytics harvest whitelist.

* Move pidfile default location search from daemon to agent.

* When the daemon is started by the agent, daemon output is now redirected
  to the agent log until the daemon successfully initializes its own log
  file. This ensures daemon startup errors are logged.

* When obfuscating SQL queries, if the start of a comment is found, replace the
  entire rest of the string with ?.

## 4.19.0 ##

### Bug Fixes ###

* Laravel transaction naming improvements.

  Prior to this version, Laravel applications that had replaced the default
  router service could find that, in some circumstances, their transactions
  would be named as "unknown" rather than being correctly named from the route.
  This has been improved: replacement router services will now get appropriate
  transaction naming provided that they either implement filtering or ensure
  that the same events are fired as the default Laravel router.

### Internal Changes ###

* The daemon config file setting `collector_host` has been eliminated.
  The collector proxy can now only only be set using the PHP ini
  `newrelic.daemon.collector_host` setting.

* The daemon/agent protocol version has been changed.
  Restarts will be required upon upgrade.

* CATMAP is now supported: we handle the extended fields in the
  X-NewRelic-Transaction header, and generate the new intrinsics required to
  power that feature.

* 'Database', 'Mongo', and 'Memcache' metrics have been changed to the
  'Datastore' namespace and have a new uniform UI.

## 4.18.0 ##

### New Features ###

* Support for New Relic Synthetics.

  The PHP agent now gives you additional information for requests from New
  Relic Synthetics when the newrelic.synthetics.enabled setting is
  enabled. More transaction traces and events give you a clearer look into how
  your application is performing around the world.

  For more details, see https://docs.newrelic.com/docs/synthetics/new-relic-synthetics/getting-started/new-relic-synthetics

* Add a new newrelic_record_custom_event API function.

  This API function allows you to add custom events to New Relic's Insights
  product.  For more information, please see:
  https://docs.newrelic.com/docs/insights/new-relic-insights/adding-querying-data/inserting-custom-events-new-relic-agents

* The daemon now uses openssl 1.0.1k, rather than 1.0.1j.

### Internal Changes ###

* The agent and daemon are now compiled with gcc 4.9.2.

* Implemented support for collector protocol 14. The agent will now apply
  segment terms to transaction names before sending them to the collector, and
  now supports non-integer agent run IDs.

* The agent/daemon protocol version has been incremented to support the
  collector protocol 14 work, and restarts will be required upon upgrade.

* Increase the transaction event harvesting rate for select accounts.

## 4.17.0.83 ##

### Bug Fixes ###

* Fix a memory leak affecting `newrelic_add_custom_parameter` API function.

## 4.17.0 ##

### Bug Fixes ###

* The agent now uses the route pattern as the transaction name for unnamed,
  closure-based routes under Laravel. For example, the following route will
  now be named "hello/{name}" instead of "GET/index.php".

  ```php
  Route::get('hello/{name}', function($name) {
    return "Hello ${name}!";
  });
  ```

* Previously the agent could fail to detect the Laravel framework when
  `artisan optimize` was used. This has been fixed.

* When the agent cannot determine a transaction name for the Laravel framework,
  the name "unknown" will be used instead of "laravel-routing-error". The latter
  implied the cause of the failure may have originated within Laravel itself
  rather than solely within the agent's instrumentation.


### Internal Changes ###

* The attribute string length limit has been changed from 256 to 255.

* A newrelic.daemon.special.curl_verbose flag has been enabled that will cause
  all cURL debug log messages to be added to the daemon log. This isn't
  expected to be generally useful, but may be a useful additional tool when
  supporting customers with unusual firewall or connection bugs.

* The agent now creates supportability metrics to track the performance/overhead
  of our output buffers. The metrics are Supportability/OutputBuffer/CAT and
  Supportability/OutputBuffer/RUM.

* The agent now discards data when the collector sends an HTTP 415 or an HTTP 413
  response as required by the agent-listener protocol.

* Add a new newrelic_record_custom_event API function.

  This API function allows you to add custom events to New Relic's Insights
  product.  For more information, please see:
  https://docs.newrelic.com/docs/insights/new-relic-insights/adding-querying-data/inserting-custom-events-new-relic-agents

  This feature has been disabled by default in this agent, in deference to APM
  stability initiatives.

* Support for New Relic Synthetics.

  The PHP agent now gives you additional information for requests from New
  Relic Synthetics when the (private) newrelic.synthetics.enabled setting is
  enabled. More transaction traces and events give you a clearer look into how
  your application is performing around the world.

  This feature has been disabled by default in this agent, in deference to APM
  stability initiatives. It has not received full QA yet.

  For more details, see https://docs.newrelic.com/docs/synthetics/new-relic-synthetics/getting-started/new-relic-synthetics

* When metric or transaction/analytics events are dumped, the deamon logs a
  message at the "warning" log level.

* Daemon shutdown internals have been greatly improved.

* All transaction trace nodes have equal priority.


### Acquia-only Notes ###

* The daemon/agent protocol version has been changed to support an upcoming feature.
  Restarts will be required upon upgrade.





## 4.16.0 ##

No public release.

## 4.15.0 ##

### New Features ###

* The daemon now uses openssl 1.0.1j, rather than 1.0.1h.

* Support for generating explain plans for SELECT queries issued using the PDO MySQL driver has been added.

  When the [newrelic.transaction_tracer.explain_enabled](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-newrelicini-settings#inivar-tt-epenabled) setting is enabled and a SELECT query is issued through a PDO connection to MySQL that takes longer than [newrelic.transaction_tracer.explain_threshold](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-newrelicini-settings#inivar-tt-epthreshold), an explain plan will be generated and will appear within the query analysis section of the [slow SQL trace](https://docs.newrelic.com/docs/apm/applications-menu/monitoring/database-and-slow-sql-dashboard#db_slowsql).

### Bug Fixes ###

* Fix an issue that could cause some background task metrics to be attributed
  to web transactions and some web transaction metrics to be attributed to
  background tasks.  This could affect Drupal customers whose applications
  use `drupal_cron_run`.

* This release changes the handling of HTTP Referer headers to always strip
  query parameters before reporting to New Relic. Previously, any query
  parameters present in a Referer header would be reported to New Relic by
  the PHP agent.

* Fix an issue where urls using the file: protocol scheme with `curl_exec()`
  would cause the agent to record external metrics. This has been corrected
  and the agent will no longer treat file urls as external services.

### Internal Changes ###

* The agent and daemon are now compiled with gcc 4.9.1.

* If the transaction events harvest message to the collector fails,
  then the events will be saved and sent to the collector during the next
  harvest period.  This should prevent gaps in transaction event data due to
  intermittent network problems.

* The daemon/agent protocol version has been changed to support the explain
  plan feature.  Therefore, restarts will be required upon upgrade.

* Guzzle 4 no longer generates duplicate metrics when using the non-multiplexed
  curl adapter.

## 4.14.0 ##

### New Features ###

* Support for agent-side configuration of application labels using the `newrelic.labels` INI setting
  has been added.

  New Relic enables you to label and organize your apps so they "roll up" into one or more meaningful
  categories. You can also easily sort, filter, and page through all the applications on your account's
  Applications list.

  See the [New Relic APM documentation](https://docs.newrelic.com/docs/apm/new-relic-apm/maintenance/categories-rollups-organizing-your-apps) and the [PHP agent (newrelic.ini) settings documentation](https://docs.newrelic.com/docs/agents/php-agent/configuration/php-agent-newrelicini-settings#inivar-labels) for more details.

### Bug Fixes ###

* Fix issue that could prevent capturing database table name for prepared
  statements on PHP 5.6.

### Internal Changes ###

* Support for creating asynchronous external nodes and metrics when using
  Guzzle 3 or 4 has been added. It can be enabled via the "guzzle" feature
  flag.

* The daemon/agent protocol version has been changed to support the labels
  feature. Therefore, restarts will be required upon upgrade.

## 4.13.1 ##

### Bug Fixes ###

* Fix an edge case segfault that could occur in situations where the daemon failed to initialize.

## 4.13.0 ##

### New Features ###

* Added support for the PHP Agent on Heroku

  We now support running PHP Applications on Heroku using the New Relic for Web
  Apps add-on.

  See the [New Relic documentation](https://docs.newrelic.com/docs/agents/php-agent/installation/php-agent-heroku) and the [Heroku documentation] (https://devcenter.heroku.com/articles/newrelic#php-configuration) for more details.

### Internal Changes ###

* Minor refactors to the daemon involving the spawn sequence and communication
  with the collector.

## 4.12.0 ##

### New Features ###

* Added two new attributes to transaction events: errorMessage and errorType.
  These attributes allow for the tracking of errors within the Insights product.
  As a result, error messages will no longer be recorded if the agent is in
  high security mode.

* Background tasks will now create Transaction events which can be tracked
  in the Insights product.

### Bug Fixes ###

* Fixed request queuing metrics support

  The PHP Agent will now allow the leading `t=` prefix to be optional when parsing
  `HTTP_X_REQUEST_START` and `X-Request-Start` headers. Previously, the header would
  be treated as absent if no leading `t=` was included.

* Fixed presentation of the newrelic.daemon.proxy setting in phpinfo()

  The `phpinfo()` function will now display a value of `no value` as the default for
  the `newrelic.daemon.proxy setting`.  Previously, it would incorrectly display a
  value of `***INVALID FORMAT***`.

## 4.11.1 ##

### Bug Fixes ###

* This update prevents proxy credentials set in the agent config file from being
  transmitted to New Relic.

## 4.11.0 ##

### New Features ###

* Our long-awaited support for Joomla 2.x and 3.x have been added. We now
  instrument and name Joomla transactions within the PHP agent without
  the need for additional plugins.


### Bug Fixes ###

* Prevent real user monitoring JavaScript injection when the Content-Type
  is first set to text/html then subsequently changed to something else.
  e.g. application/json.

* Fix Zend Framework transaction naming when `exit()` is called during request
  processing. Previously, it was possible to exit before the agent set the
  transaction name. This caused the transaction to be reported as 'index.php',
  and most commonly occurs when the Redirect action helper is used. The agent
  will now correctly name the transaction when this happens.

* Fix segfault triggered by JSON encoding when strings to be encoded are marked
  as UTF-8, yet contain invalid UTF-8 byte sequences.


### Internal Changes ###

* Improved logging when `debug_autorum` is enabled. The agent now logs changes
  to the current mime type, for example when `header()` or `header_remove()`
  are called, as well as logging the current Content-Type header and mime type
  at the time RUM injection is performed. The agent will also log manual output
  buffer flushes. e.g. `ob_flush()`.

* The default signal handler will be restored and executed when a fatal signal
  (segfaults, bus errors, et al) is handled. This allows for easier access to
  core dumps, as the user will no longer have to set
  `newrelic.special = no_signal_handler`.

### Acquia-only Notes ###

* This is the first GA release that contains the fix for the RUM injection issue
  that affected GaiamTV.

* There is no longer a need to set `newrelic.special = no_signal_handler` in
  order to generate core dumps, as the signal handler installed by the agent
  will re-raise the signal with the default handler.

## 4.10.1 ##

### Bug Fixes ###

* Fixed an issue whereby Debian and Ubuntu users running the daemon via the
  init script may have been unable to start the daemon after upgrading to
  4.10.0. This has been fixed by reverting back to the init script used in
  4.9.0 and earlier versions.

## 4.10.0 ##

### End of Life Notices ###

* The MacOSX/Darwin agent and daemon are now compiled only for MacOSX 10.6 or later.

### New Features ###

* The daemon now uses openssl 1.0.1h, rather than 1.0.1g.

* Logging to stdout and stderr is now supported when running the daemon in the foreground.

* Created new installer for Ubuntu/Debian systems. If the application name is not present, the installation process will ask for one. Also, Debian/Ubuntu users do not have to run the newrelic-install script.

### Upgrade Notices ###

* The agent now supports PHP 5.6beta4.

### Bug Fixes ###

* Zend externals could be double counted when the cURL adapter was used.
  In previous versions of the agent, using `Zend_Http_Client::setAdapter()`
  with an instance of `Zend_Http_Client_Adapter_Curl` caused the agent
  to double count the external call. This has been fixed.

* Prevent real user monitoring javascript injection when a Content-Type
  header has not been set and the default mimetype is not text/html.

### Internal Changes ###

* newrelic-install will now ignore any startup errors generated by a PHP binary
  when checking the PHP version and extension directory.

* Users installing the newrelic-php5 Debian packages on a system with the
  standard php5 packages installed will be prompted on installation for their
  license key and application name, and will not have to run newrelic-install
  separately after installation. Users who aren't using the standard packages
  will be told to run newrelic-install once package installation is complete.

### Acquia-only Notes ###

* This release does not contain the fix for the RUM injection issue that
  affected GaiamTV.

## 4.9.0 ##

### New Features ###

* Added new local setting to enable high security mode.  ```newrelic.high_security = false```

  This setting is false by default.  When set to true, this setting has the following effects:

  * Data will not be sent to New Relic unless the newrelic-daemon is using SSL.

  * Raw SQL strings will never be gathered, regardless of the value of ```newrelic.transaction_tracer.record_sql```.

  * Request parameters will never be captured, regardless of the value of ```newrelic.capture_params```.

  * The following API functions will have no effect, and will return false:
    newrelic_add_custom_parameter
    newrelic_set_user_attributes

  If you change this setting, you must also change the RPM UI security setting.
  If the two settings do not match, then no data will be collected.

* Introduce new attribute configuration scheme.

  All parameters captured by the agent and created by the ```newrelic_add_custom_parameter```
  API call are now considered attributes and have a new set of INI configuration options.
  As a result, the ```newrelic.capture_params``` and ```newrelic.ignored_params``` configuration
  options have been deprecated.  The ```newrelic.ignored_params``` comparison will now be case-sensitive.

  For full information, please see https://docs.newrelic.com/docs/subscriptions/agent-attributes

### Bug Fixes ###

* Fix potential metric grouping issue with Kohana transaction naming.
  Previously, the agent would treat the first two segments of the request
  URI as the controller and action when no route was found. Now the agent
  will use 'unknown' as the transaction name when no route is found. This
  is consistent with the behavior for other frameworks.

### Internal Changes ###

* Limited the number of metric cache entries to prevent memory accumulation during
  metric explosion.

* The introduction of the ```newrelic.high_security``` setting has changed the agent-daemon
  protocol version.  Therefore, the daemon will need to be restarted upon
  upgrade.

### Acquia-only Notes ###

* The new attributes changes in this release are designed towards
  standardizing attributes (formerly called custom parameters) across our
  agents for Insights, New Relic's beta product for real time software
  analytics. Attributes will show up as facets for data queries in
  Insights. Some page view attributes are available by default, such as
  ```userAgentName``` but customers can also create custom attributes in
  order to support custom facets for NRQL queries in Insights.

  More information about attributes and Insights is available on our docs site: https://docs.newrelic.com/docs/features/agent-attributes.

* This release does not contain the fix for the RUM injection issue that
  affected GaiamTV.

## 4.8.2 ##

### Bug Fixes ###

* Prevent real user monitoring javascript injection when the Content-Type
  is first set to text/html then subsequently changed to something else.
  e.g. application/json.

### Internal Changes ###

* Improved logging when `debug_autorum` is enabled. The agent now logs changes
  to the current mime type, for example when `header()` or `header_remove()`
  are called, as well as logging the current Content-Type header and mime type
  at the time RUM injection is performed. The agent will also log manual output
  buffer flushes. e.g. `ob_flush()`.

### Acquia-only Notes ###

* This release fixes the RUM injection issue that affected GaiamTV.

## 4.8.1 ##

### Bug Fixes ###

* Prevent real user monitoring javascript injection when a Content-Type
  header has not been set and the default mimetype is not text/html.

## 4.8.0.47 ##

### New Features ###

* The daemon now uses libcurl 7.35.0, rather than 7.30.0.

* The daemon now uses openssl 1.0.1g, rather than 1.0.1e.

* Added support for PHP 5.6beta.

### Bug Fixes ###

* Fixed an issue that would cause New Relic cross application tracing headers to accumulate during
  multiple ```curl_exec``` calls.

* Fixed an issue with the installer when the path includes spaces.

  When newrelic-install.sh was run from a path that included spaces, such as
  ```/tmp/new relic/newrelic-php5-4.6.5.40-linux```, the installation would
  fail. This has been corrected.

* Fixed an issue where not all Drupal 8 hooks were instrumented.

  While most Drupal 8 hooks are executed via ```ModuleHandlerInterface::invoke()```
  and ```ModuleHandlerInterface::invokeAll()```, a handful such as ```page_build```
  are executed via variable functions by Drupal 8 as they require their arguments
  to be passed by reference. These hooks are now instrumented correctly.

### Internal Changes ###

* The default ```newrelic.special.max_nesting_level``` has been changed to -1.  Therefore
  the stack nesting level clamp is now disabled by default.

* Filename-based framework detection is now case-insensitive.

### Acquia-only Notes ###

* As summarized in the Internal Changes section above, this
  release includes a change to the default setting for the
  "Stack Clamper" - a feature that was introduced to address
  a problem with a menu recursion bug in this application:
  https://rpm.newrelic.com/accounts/187398/applications/1156098 .

  You will want to ensure that the value for setting value for
  ```newrelic.special.max_nesting_level``` is set to an appropriate value.
  This feature originally shipped with a setting value of ```500``` but
  later versions of the agent updated it to ```5000``` which worked well
  for most customers whereas the original setting caused many customers
  to experience truncated transaction traces to be captured in New Relic.
  In other words, the setting prevented many customers for seeing all
  their application data in New Relic. Overall this setting has been
  deemed to be useful for sites that exhibit this problem, but for many
  of our non-Acquia customers, this feature has been problematic and has
  generated a lot of support for us, which is why we have disabled it in
  this version as the default.

* This release includes instrumentation of more Drupal hooks
  for Drupal8. Most Drupal hooks are invoked via ```module_invoke()```
  and ```module_invoke_all()``` in Drupal 6 and 7, and their analogues
  ```ModuleHandler::invoke()``` and ```ModuleHandler::invokeAll()``` in Drupal 8.
  We rely on this when generating module and hook metrics: we directly
  instrument the invoke functions, and wrap ```call_user_func_array()``` to
  indirectly instrument invocations via invokeAll by examining the caller.

  There are a number of places in both Drupal 7 and 8 where this
  assumption doesn't hold, however: due to PHP no longer supporting
  call-time pass-by-reference and Drupal's reliance on by-ref arguments
  in some hooks, Drupal code has to call the hook functions directly,
  bypassing the invocation functions. This means that these hooks never
  generate metrics, and that module metrics may incompletely account for
  the time spent in the module.

  This release changes the way we measure these metrics by directly
  instrumenting the hook functions themselves. This has some notable
  issues: we have to track additional information in the per-request
  global structure, and we end up instrumenting a lot of additional
  functions (about 100 for a typical Drupal 8 page), which may have
  a performance impact. Our benchmarking found that when the server
  is overloaded with requests, the new agent caused a sharp drop in
  performance, with 4.8 providing 71% of the throughput of 4.7. However,
  under a more reasonable load (but still pinning CPU use at 100% on the
  server), the 4.8 agent was 98% of the throughput of 4.7.

  This new method has a really dramatic effect on the contents of the
  Drupal->Modules displays and Drupal->Hooks displays without noticeably
  affecting agent response time overhead. While this is a big improvement,
  showing much more useful information to the customer, it might be hard
  to explain to customers who are actually comparing between upgrades.

  Note that this additional instrumentation is only going to be
  implemented for Drupal8. Tackling this for Drupal6/7 is not planned -
  we feel that the changes in module, hook, and view timings might be too
  much cheese-moving for D6/7 customers between agent upgrades, whereas
  someone building a D8 site is going to expect things to be different.
  In addition we expect that the performance affects of this additional
  instrumentation will be more severe in D6/7.

  This list of additional hooks that will show up for Drupal 8 sites include:
  * ```entity_load```
  * ```${entity}_load```
  * ```theme```
  * ```cron```
  * ```page_build```
  * ```schema```
  * ```watchdog```
  * ```menu_local_tasks```
  * ```menu```
  * ```help```
  * ```search_preprocess```
  * ```permission```
  * ```rdf_namespaces```
  * ```node_validate```
  * ```node_submit```
  * ```field_attach_form```
  * ```field_attach_extract_form_values```
  * ```file_download_access```
  * ```filetransfer_info```
  * ```url_inbound_alter```
  * ```user_$type```
  * ```node_info```
  * ```field_info```
  * ```field_widget_info```
  * ```field_formatter_info```
  * ```field_storage_info```
  * ```field_storage_pre_load```
  * ```field_attach_validate```
  * ```field_attach_submits```
  * ```field_storage_pre_insert```
  * ```field_storage_pre_update```
  * ```field_update_forbid```
  * ```field_attach_purge```
  * ```field_access```
  * ```filter_info```
  * ```image_default_styles```
  * ```image_effect_info```
  * ```block_info```




## 4.7.5.43 ##

### New Features ###

* Support for custom SSL trust stores

  It is now possible to configure the agent to use a custom SSL trust
  store to authenticate New Relic's servers. In most cases this should
  not be necessary. The agent ships with the necessary certificates and
  will use them unless configured not to do so. Caution should be used
  with these settings. If the trust store does not contain the necessary
  certificates, the agent will be unable to report data to New Relic.
  See the [documentation](https://docs.newrelic.com/docs/php/php-agent-phpini-settings#inivar-daemon-settings)
  for more details.

### Bug Fixes ###

* ```drupal_http_request``` responses that redirect are no longer double
  counted

  External metrics created when ```drupal_http_request``` received a HTTP
  response that was a redirect, such as a 302 Found response, would have their
  execution time counted and call count incremented twice. The double counting
  has been removed, and the entire external time will be counted including any
  redirects.

* Fixed potential OpenSUSE 11.4 segmentation fault during Apache graceful restart

## 4.6.5.40 ##

### End of Life Notices ###

* This release no longer supports BSD (either 32-bit or 64-bit) or the
  32-bit variant of Mac OS X.

### New Features ###

* Add Laravel 4.0 support

  Laravel version 4.0 or greater is now supported natively. The naming
  scheme that we use follows the scheme developed by Patrick Leckey
  at Intouch Insight, and we are very grateful for his assistance.

  If you are already using the LaravelNewrelicServiceProvider from
  https://packagist.org/packages/intouch/laravel-newrelic then we
  recommend removing that from the list of service providers, and
  relying on our native support instead.

  However, if you are using the custom naming mechanism with
  ```name_provider``` from the intouch/laravel-newrelic package, then
  those transaction names will take precedence over our automatically
  generated transaction names. If you do use ```name_provider```, please
  write us and let us know what your namer does so we might extend our
  built in capabilities.

### Bug Fixes ###

* Improved real user monitoring Javascript insertion.

  The real user monitoring Javascript loader will now be injected after
  any meta charset tag, ensuring that the meta charset tag is
  close to the beginning of the page.

  The real user monitoring Javascript footer will no longer be inserted
  into HTML comments or nested HTML iframes.

* Properly observe ```newrelic.ignored_params``` when capturing the request's URL.

  If ```newrelic.capture_params``` is true then the agent will capture the values
  within the ```$_REQUEST``` parameter and the full raw URL of the request.  The
  contents of ```newrelic.ignored_params``` are used to selectively ignore values
  from ```$_REQUEST```.  The presence of ```newrelic.ignored_params``` will now also
  prevent the capturing of query parameters in the request URL.

* Fix transaction naming when newrelic.webtransaction.name.functions is present.

  When newrelic.webtransaction.name.functions had a non-empty value, the
  URL was being used as the default web transaction name instead of the PHP
  filename.

* Print backtrace to log file in the event of a segmentation fault.

  This functionality has been missing since release 4.4.

## 4.5.5.38 ##

### End of Life Notices ###

* This release no longer supports any backdoor exceptions to run
threaded Apache MPM.  PHP itself is unstable in this environment.

* This is the last release of the PHP agent for Solaris (either 32-bit or 64-bit),
BSD (either 32-bit or 64-bit) or the 32-bit variant of Mac OS X.

* This release no longer ships with any backdoor support for PHP 5.1.

### New Features ###

* Add Drupal 8 support

  Drupal 8 is now fully supported by the PHP agent's framework detection code.
  Drupal 8 applications will have their transactions named correctly, and will
  also generate the same module, hook and view metrics as Drupal 6 and 7
  applications. This support can be forced by setting the
  ```newrelic.framework``` configuration setting to ```drupal8``` if
  auto-detection fails.

### Upgrade Notices ###

* All customers running on old x86 hardware that does not support
the SSE3 instruction set (such as early releases of the AMD Opteron)
should upgrade to this release as soon as possible.  Prior to this
release, the daemon inadvertently contained SSE3 instructions which
would cause an illegal instruction on such hardware.  The only way
the SSE3 instructions were executed was when we changed the choice
and priority of SSL cipher algorithms at our data center, and we
would like to change those priorities by the end of 2Q2014.

### Bug Fixes ###

* Fixed a bug with ```file_get_contents``` instrumentation.

  Fixed a bug which would cause the default context to be ignored by
  ```file_get_contents``` when a context parameter was not provided.

## 4.4.5.35 ##

This release of the New Relic PHP Agent improves real user monitoring
and adds support for custom parameters in transaction analytics
events. These changes were made to create a solid foundation for
future enhancements. Additionally, an emphasis was placed on reducing
the download size of the agent. As a result, the download size has
been reduced by 75%.

### New Features ###

* Improve Real User Monitoring

  The PHP agent's code for both automatic and manual injection of Real
  User Monitoring scripts has been improved. No application changes are
  required, but the new injection logic is simpler, faster, more robust,
  and paves the way for future improvements to Real User Monitoring.

* Attach custom parameters to analytics events

  Custom parameters given to the agent through the API functions
  ```newrelic_add_custom_parameter``` and
  ```newrelic_set_user_attributes``` can now be automatically attached
  to transaction and page view analytics events. By default custom
  parameters will be attached to traces, errors, and transaction
  analytics events. Optionally, custom parameters can also be included
  in the JavaScipt injected for Real User Monitoring. This allows
  custom parameters to be attached to page view analytics events.

  The behavior of custom parameters can be controlled through the
  following configuration settings, which are shown with their
  default values.

  ```
  newrelic.transaction_tracer.capture_attributes = true
  newrelic.error_collector.capture_attributes = true
  newrelic.analytics_events.capture_attributes = true
  newrelic.browser_monitoring.capture_attributes = false
  ```

* Time other extensions' shutdown functions

  Users can enable the timing of extensions' shutdown functions using
  the private ini setting: ```newrelic.special.enable_extension_instrumentation = true```
  This setting will have two effects:  Request duration will increase, since
  timing will stop at post-shutdown rather than shutdown, and
  the shutdown function calls of other extensions will appear in the
  transaction trace.  This will allow the agent to possibly diagnose
  unexplained time at the end of transaction traces.

## 4.3.5.33 ##

This release of the PHP agent adds support for SOAP external calls,
automatically marks Drupal cron requests as background tasks, improves
the categorization of PHP errors within the New Relic UI, and fixes
several bugs. Additionally, the internals of the agent have been
significantly refactored to improve the stability and performance
of the agent.

### New Features ###

* Support for SOAP external calls

  Calls made to ```SoapClient::__doRequest()``` will now be counted as
  external calls. This enables Cross Application Tracing support for
  SOAP APIs.

  See https://docs.newrelic.com/docs/traces/cross-application-traces

* Treat Drupal cron requests as background tasks

  Drupal cron requests (identified by calls to ```drupal_cron_run()```)
  will now be counted as background tasks (not web transactions) regardless
  of how they are started. Previously, it was necessary to use the
  ```newrelic_background_job()``` API to explicitly mark Drupal cron
  requests as background tasks.

* Use the exception class as the category for errors

  Errors created using the ```newrelic_notice_error()``` API function with
  an exception will be grouped according to the exception's class.
  Previously, all errors were grouped under 'NoticedError'.

See https://docs.newrelic.com/docs/php/the-php-api#api-notice-error

* Limit the maximum number of nested PHP function calls

  The agent will now limit the maximum number of nested function calls
  within PHP applications. This is to provide protection against stack
  overflows due to infinite recursion. Previously, this would result
  in a segmentation fault. The agent will now log an error and
  terminate the process by raising a fatal PHP error. By default,
  this limit is set to a very high value of 500. Exceeding this limit
  should be extremely rare.

* Improve the stability and performance of the agent

  The internals of the agent and its background daemon process have been
  significantly refactored to improve interprocess communication, stability
  and performance. The maximum number of connections to the background
  daemon process has been doubled to 4096.

### Bug Fixes ###

* Fix for installing on Ubuntu 13.10 when using PHP-FPM

  Previously the installer would fail to install a newrelic.ini file in
  the configuration directory for PHP-FPM (/etc/php5/fpm/conf.d), if one
  was present. This has been fixed, and the installer will correctly
  install a newrelic.ini for CLI, DSO and FPM configuration directories.

* Fix for ```drupal_http_request()``` always returning ```NULL``` under PHP 5.5

  Version 3.9.5.13 added support for external calls made using
  ```drupal_http_request()```. Due to the way the agent instruments this
  function under PHP 5.5, this function would always return ```NULL```
  instead of the correct return value. This has been fixed.

* Fix for real user monitoring when a Content-Length response header is present.

  When automatic real user monitoring is enabled, the agent will only perform
  JavaScript injection into HTML pages when a Content-Length header is not
  already present in the response. In some cases, this check was not working
  correctly leading to the value in the Content-Length header not matching the
  actual number of bytes sent to the client.

* Fixed a segmentation fault when Zend Thread Safety is enabled.

  Attempts to use the agent under Apache using a threaded MPM (e.g. worker
  or event) and ```mod_php``` would result in segmentation faults. This has been
  fixed, but New Relic [does not support][0] using the agent in these
  circumstances.

* Fixed high CPU usage under heavy load

  Under very high loads with thousands of PHP processes, the background
  daemon process could consume 100% CPU when the per-process file
  descriptor limit was exceeded. This has been fixed. The agent will
  now throttle the number of new connections accepted when per-process
  file descriptor limit is reached. A message is also printed to the
  log file.

[0]: https://docs.newrelic.com/docs/php/php-and-the-apache-worker-mpm
