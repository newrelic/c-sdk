# Using PHP

A full introduction to PHP is well beyond the scope of this documentation. In this chapter, we're going to focus mostly on how our customers would tend to stand up a new PHP application so that you can get a feel for what issues they're going to work through.

The most useful resource when following along with existing code is likely the [PHP reference manual](https://secure.php.net/manual/en/): it's the definitive guide to the behaviour of every language feature and function in PHP. It's not a guide or tutorial, but if you ever need to know what something does, it's the quickest way to get there. Bonus pro tip: you can search for functions and classes by name directly via the URL; for example, to get the reference page for the `date` function, you can simply go to [https://secure.php.net/date](https://secure.php.net/date).

## PHP installation

PHP is available in every common (and most uncommon) Linux distributions. Most of our users use CentOS/RHEL or Debian/Ubuntu, and we provide PHP agent packages for both.

Since most users will want to use PHP to drive a Web site, it's common to install PHP alongside a Web server. The most common choices by far are [Apache 2](https://httpd.apache.org/) and [nginx](http://nginx.org/): both allow PHP to be used via FastCGI, and Apache also supports using PHP via an Apache module.

A basic PHP installation only includes a relatively small part of the standard library: most functionality is provided via extensions, which are written in C (like PHP itself). The [PHP source tree](https://github.com/php/php-src) includes a variety of bundled extensions, which can be enabled or disabled at compile time, and there are also many other extensions available as third party addons. (Including the PHP agent.)

Distribution packages break PHP down into its core, which is generally available via a `php` or `php-common` package, then provide other packages for server APIs (more on those in a moment) and extensions (for example, PHP's 2 or 3 extensions for accessing MySQL are generally wrapped up in a `php-mysql` package).

Once installed, PHP is configured via `.ini` files. PHP can tell you where it's looking for configuration either by executing the [`phpinfo()`](https://secure.php.net/phpinfo) function, which outputs a large amount of runtime and configuration data, or by running `php --ini` on the command line. The [deployment chapter](deployments.md) includes more detail on how PHP is configured in common cases.

### Server APIs

The core of PHP — that is, the VM and core parts of the standard library — is actually built as a static library, which is then linked into a "server API"<sup id="a0">[0](#f0)</sup>, which exposes that core to the outside world.

Server APIs bundled with PHP and in common use include:

* `cli`: this provides a `php` binary that can be installed to `/usr/bin` (or wherever) and run as a normal binary, much like `python` or `ruby`.
* `cgi`: much the same, except this provides a `php-cgi` binary for old fashioned [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface) handlers. This is not in common use any more, but is a lowest common denominator way of using PHP from a dumb Web server, since _everything_ supports CGI.
* `fpm`: this provides a `php-fpm` daemon that provides a configurable pool of processes to handle requests via [FastCGI](https://en.wikipedia.org/wiki/FastCGI). This is the most common way of deploying PHP on modern stacks: it's easy to get running, more performant than `apache2handler` (described below), can be separated from the Web server entirely if need be via a container, chroot, and/or different user, and integrates well with init systems.
* `apache2handler`: this provides an Apache 2 module that runs within `httpd` processes in the Apache process pool and can be used as a request handler within the Apache configuration. This was the traditional way of deploying PHP for reasonable performance, and is still commonly seen, particularly on older stacks.

## Frameworks

Although PHP provides all of the functionality required to implement a Web site backend within the language, almost all sites use a framework to provide the scaffolding for request routing, templating, database access, session handling, and other annoying tasks that developers don't really want to have to implement from scratch using the standard library.

Early PHP frameworks tended to be closer to what we would call content management systems today: they were opinionated, full featured systems that provided significant builtin functionality. [phpnuke](https://www.phpnuke.org/), for example, was an early project that provided a full CMS, news system, and forum alongside a truly astonishing number of security issues. Users could write their own modules that plugged into phpnuke for their own functionality.

Some frameworks have survived to the modern day with this structure: [WordPress](https://wordpress.org) started out as a blog platform, but now has a huge ecosystem of plugins that provide basically any functionality you can think of for a Web site, and [Drupal](https://drupal.org/) has also evolved past being a basic brochureware CMS to being a pluggable framework used for many different types of site.

After Ruby on Rails was released, frameworks that provided more barebones scaffolding appeared. For example, [CakePHP](https://cakephp.org/) was an early RoR clone, while the [Zend Framework](https://framework.zend.com/) was an attempt at a more loosely coupled component library that could be plugged together into a bespoke framework that suited the problem at hand.

In recent times, the trend has been towards modular, but opinionated frameworks. [Symfony](https://symfony.com/) and [Laravel](https://laravel.com/) are the two best known examples of this type of framework: neither provides any CMS functionality out of the box, but both provide everything you would need to build a CMS (or whatever you're doing) very rapidly.

PHP developers will generally start with a blank project in their preferred framework. Indeed, many developers tend to consider themselves a "$framework developer" before a "PHP developer": while the language they're writing in is still PHP, they're usually using it to interact with components and services provided by the framework or framework modules more than the actual standard library.

### The old school

We'll use [WordPress](https://wordpress.org/) as an example, since it's our most popular framework. (Drupal 6 and 7 are broadly similar in concept.) Older frameworks date from a time before PHP had robust dependency management, so the process is generally fairly manual:

1. Install PHP with a Web server and a database server. On recent versions of Ubuntu, this is about as simple as `apt-get install php mysql-server libapache2-mod-php apache2`, if you're happy with using `apache2handler`.
2. [Download](https://wordpress.org/download/) a tarball or zip file containing the latest release.
3. Extract the file to somewhere in the document root (`/var/www/html`).
4. Create a MySQL<sup id="a1">[1](#f1)</sup> database and grant permission to manipulate it to a user.
5. Access the install script via the Web server (most likely by going to http://server.name/wordpress/ or similar), and configure the database. If WordPress has write access to the document root, it will write a configuration file, otherwise you have to copy it into the right place.

At that point, you're ready to start developing whatever custom functionality you need: on WordPress, that's through a plugin.

### The new school

Newer frameworks almost exclusively use [Composer](https://getcomposer.org/), which is a dependency management system akin to npm or Ruby gems<sup id="a2">[2](#f2)</sup>. Like those systems, this means that third party code generally lives in a `vendor` directory, and the user deploys a skeleton that provides the basic scaffolding required to build atop.

Composer itself can also provide the skeleton. For example, to set up a [Laravel](https://laravel.com/) project, you can use the following:

```sh
composer create-project --prefer-dist laravel/laravel project-dir
```

This places a skeleton in the `project-dir` directory, including installing Laravel's library requirements via Composer and setting up basic front controllers to handle requests. Laravel also provides a development server built on top of PHP's own development server, which can be run using `artisan`, a task runner provided by Laravel<sup id="a3">[3](#f3)</sup>:

```sh
cd project-dir
./artisan serve
```

At that point, the skeleton is listening on http://localhost:8000/ and provides a default route, along with examples of how to write your own controllers, models, and views.

Note that we didn't set up a database or Web server: while instructions are provided in [Laravel's manual](https://laravel.com/docs/5.4) to do so, a Web server is generally only required for production deployments, and a variety of databases can be configured if required through Laravel's configuration system. Modern PHP frameworks tend to be very loosely coupled.

## Libraries

Of course, frameworks can't provide everything an application needs. PHP also has a large ecosystem of libraries available.

Until about five years ago, libraries tended to be installed in one of two ways: either via direct downloads that needed to be extracted into a project and manually included, or via [PEAR](https://pear.php.net/), an early attempt at a [CPAN](http://cpan.perl.org/) style centralised repository of "blessed" PHP code, but which could only install said code on a system-wide, rather than per-project, basis.

As mentioned above, [Composer](https://getcomposer.org/) is the modern solution: it provides per-project dependency management via a JSON configuration file, much like [npm](https://www.npmjs.com/), and allows repeatable builds via lock files.

The default package repository used by Composer is [Packagist](https://packagist.org/), which is closer to npm in spirit than CPAN: packages (which may be libraries or full-blown frameworks) can be published by anyone with an account.

## Footnotes

<sup id="f0">0</sup> Also commonly called a SAPI. This is a terrible name, and hasn't really described what's going on since before PHP was able to be run from the command line, but we're stuck with it. [↩](#a0)

<sup id="f1">1</sup> PHP has been joined at the hip with [MySQL](https://www.mysql.com/) in common developer culture for many years: the [LAMP stack](https://en.wikipedia.org/wiki/LAMP_(software_bundle)) is famous for this. PHP actually supports almost every database in common use either via a bundled extension or, at worst, an easily installed third party extension, but MySQL was in the right place at the right time to get the mindshare, and many older frameworks (including WordPress) are heavily dependent on MySQL specific quirks. More modern frameworks tend to support any relational database that uses [PDO](https://secure.php.net/pdo), which is a common API for database access along the same lines as JDBC. [↩](#a1)

<sup id="f2">2</sup> Most newer distributions include Composer in packaged form (usually via something like `php-composer`), but as it only had a stable release in the relatively recent past, distributions that are more than about two years old usually don't have it available, in which case it must be installed from [its Web site](https://getcomposer.org/). [↩](#a2)

<sup id="f3">3</sup> Modern frameworks generally provide a command line task runner in one form or another; these are almost always built using [Symfony's Console component](http://symfony.com/doc/current/components/console.html), and can be recognised by the way they're invoked, which always takes a command name as the first argument. [↩](#a3)
