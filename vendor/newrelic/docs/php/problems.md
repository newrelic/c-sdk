# Common problems

## How is PHP configured?

The easiest way to find this out is via [`phpinfo()`](https://secure.php.net/phpinfo): running a PHP script that consists entirely of the following will get you the details:

```php
<?php phpinfo();
```

If you want to know where PHP is looking for its configuration, this is shown early in the output; this includes both the location of the master `php.ini` and any scan directories in use.

For command line PHP, you can also run `php -i` to get the same output, and `php --ini` to just get the loaded configuration files.

## How do I find this definition in the Zend Engine?

If you have an IDE with good C support, a quick way is to load the php-src branch you're interested in into the IDE.

For a quicker solution, there are also online tools to find definitions and search the PHP source across versions: the PHP project is supposed to maintain a LXR instance at [http://lxr.php.net/](http://lxr.php.net/), but in practice it's down more often than it's up. Adam runs a mirror at [https://php-lxr.adamharvey.name/source/](https://php-lxr.adamharvey.name/source/) that is more likely to be available and definitely covers all the branches we need for the PHP agent.

## What the hell is this macro?

The aforementioned LXR instances should be able to help with this too. Inscrutable macros with terrible, short names tend to be from the older days of PHP, so _Extending and Embedding PHP_ is often a good bet here too.

## How do I quickly stand up a PHP site?

PHP's built in Web server is often the quickest, dirtiest way to get something going, since you can configure everything. To get a HTTP server listening on port 8000 with the current working directory as the document root:

```sh
php -S 0.0.0.0:8000
```

## How do I quickly stand up a PHP site with a particular PHP agent and get some logs?

Expanding on the above, you can tell PHP to ignore any configuration it has and load an agent from a `php_agent` tree with something like the following:

```sh
php -n -d extension=/path/to/php_agent/agent/.libs/newrelic.so -d newrelic.daemon.collector_host=staging-collector.newrelic.com -d newrelic.license=YOUR_STAGING_LICENCE_KEY -d newrelic.logfile=stdout -d newrelic.loglevel=verbosedebug -S 0.0.0.0:8000
```

This will output verbosedebug logs to standard output, which is often useful.