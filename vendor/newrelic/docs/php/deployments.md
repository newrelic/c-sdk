# Common deployment types

This chapter provides a rough guide to the common ways users install, configure, and deploy PHP. When dealing with support tickets, knowing these will greatly simplify debugging configuration issues.

## Ubuntu/Debian

Newer versions of Ubuntu and Debian ship with PHP packages that are designed to permit side by side installs of different PHP versions. On Debian 8, the PHP 5.6 packages are in the `php5-*` namespace; on Ubuntu 16.04 and newer and Debian 9 onwards, PHP 7.x packages are in the `php7.x-*` namespace (at the time of writing, either `php7.0-*` or `php7.1-*`; `apt-cache search ^php7` should tell you what's available).

These packages are configured to allow sharing extension configuration between different SAPIs on the same version: the master `php.ini` for each SAPI is found in `/etc/php/$branch/$sapi/php.ini` (for example, PHP 7.1 CLI would be in `/etc/php/7.1/cli/php.ini`), and each SAPI then gets a scan directory where extension configuration can be symlinked (for example: `/etc/php/7.1/cli/conf.d`). The intention is that users should never directly manipulate these directories: extension configuration should be placed into per-extension `.ini` files in `/etc/php/7.1/mods-available`, and then the `phpenmod` and `phpdismod` scripts provided by the `php-common` package are used to configure the symlinks.<sup id="a0">[0](#f0)</sup>

Users who want to use more up to date PHP versions on their Ubuntu or Debian installs generally use [Ondřej Surý's packages](https://deb.sury.org/) via his PPA or DPA, respectively. These packages are laid out the same as the official packages (mostly because he maintains the official packages), and can be installed alongside the official packages.

Historically, [Dotdeb](https://www.dotdeb.org/) was another common source of third party PHP packages, but they are gradually winding back their operations, and anecdotally, most users have switched to Ondřej's packages when they need newer versions.

Most users will either load PHP as an Apache module or use PHP-FPM with nginx via FastCGI: both of these use cases are supported by the packages shipped with the distros. In the Apache case, installing `libapache2-mod-php7.1` and restarting Apache gets the job done; in the nginx case, this involves installing `php7.1-fpm` and uncommenting the parts of the nginx site configuration relating to PHP, then restarting nginx.

## Red Hat/CentOS

As with everything else, RHEL and CentOS ship horrifically old versions of PHP (the _current_ version of RHEL ships PHP 5.4 at the time of writing, which was EOLed over two years ago). RHEL/CentOS 5 ship PHP 5.1.6 by default, which we haven't supported for some time, but PHP 5.3.3 packages are available in the `php53-*` package namespace.

The RPM packages tend to be less magic than the DEB packages: configuration isn't split by SAPI or version, and is just lumped in `/etc/php.ini` and whatever files are found in `/etc/php.d`.

As with Debian and Ubuntu, running PHP as an Apache module is handled automatically if the `php` and `httpd` packages are installed. nginx configuration is more manual, starting with the fact that nginx isn't available in the official repos, so it's less common for RHEL/CentOS users to use nginx with PHP.

## Docker

Docker Hub provides a wide range of PHP variations in the [`php` repository](https://hub.docker.com/_/php/). These include all currently supported versions, the common SAPIs, and Alpine or Debian base distros.

New Relic is currently quite behind the times on providing one step installs of the PHP agent that can work with these images. However, most users will need to write a `Dockerfile` to configure their PHP install, since these images are quite minimal in terms of the extensions that are provided, so they can them follow the installation instructions on our docs site to handle the install.

These images are a very quick way of standing up a near production ready PHP configuration, and are quite popular as base images.

## Bundles

Historically, there was a thriving ecosystem of all-in-one bundles that included a Web server (almost always Apache), PHP, and a database (almost always MySQL) in a single installer. Commonly used bundles included [WAMP](http://www.wampserver.com/en/) for Windows, and [MAMP](https://www.mamp.info/en/) for both macOS and Windows.

The popularity of these has dwindled as PHP has become easier to install and it has become less common to use MySQL and Apache, but many older developers will still use them to quickly set up local development environments.

## Custom builds (ie: the rest)

PHP can be built from the source tarballs on php.net using the standard autotools `./configure && make && make install` method on non-Windows platforms. In those cases, everything is configurable, and nothing is set.

In those cases, the easiest way to find out how PHP was built is through `phpinfo()`: almost all options are handled as flags given to `./configure`, and the full command line given to `./configure` is shown in `phpinfo()` output.

## Footnotes

<sup id="f0">0</sup> For Debian 8 only with PHP 5.6, there are some subtle naming differences: the configuration path is `/etc/php5` instead of `/etc/php`, and the module handling scripts are `php5enmod` and `php5dismod` instead of `phpenmod` and `phpdismod`, respectively. How it all works is the same, however. [↩](#a0)