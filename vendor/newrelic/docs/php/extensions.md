# PHP Extensions

Back in the [using PHP](using.md) chapter, we talked briefly about extensions, but more about PHP libraries and frameworks. Now we'll talk about extensions in detail: the PHP agent is itself a PHP extension, so knowing how they work is crucial to understanding it.

## Extensions

The core of PHP is the Zend Engine, which provides a compiler that turns PHP source code into bytecode, a VM to run the bytecode, and the basic functionality required to support PHP's [built in types](https://secure.php.net/types). The Zend Engine itself provides very little else, though: only a handful of PHP functions to enable debugging and monitoring, but nothing that would be considered any sort of standard library.

The reason for this is that almost all of PHP's standard library and functionality are actually provided through extensions. In PHP parlance, an extension is a module written in C that uses the C API provided by the Zend Engine to implement functionality in the form of classes, methods, functions, and anything else that's required internally to make that functionality work. PHP provides a build system that is used to build these extensions either as static libraries (which, for extensions bundled with PHP itself, are usually then linked directly into the server API that's being built) or shared libraries, which are then loaded using the `extension=` configuration directive.

In addition to being able to define functions and classes, extensions also get access to a variety of hooks through the Zend Engine API: they can hook into or even replace many parts of the VM, and can gain visibility into the inner workings of the VM. This is crucial in PHP, as unlike most other common scripting languages, code written in PHP has remarkably few tools available to monitor or change how PHP executes code.

Ergo, the easiest way to write a profiler is to build an extension, which is exactly what we did for the PHP agent.

## Installing extensions

Many extensions are available within the PHP source code: they are enabled or disabled by giving flags to the `configure` script when building PHP, and are generally statically compiled into PHP.

There are many more extensions available that aren't bundled with PHP, however, including the New Relic PHP agent. For open source extensions (which doesn't include us, but does include almost every other extension in common use), there are two main ways of installing them.

### PECL

Earlier, we mentioned PEAR, which was an attempt at building a CPAN style central repository for PHP code. It was moderately successful, but has been supplanted since by better tools than can handle per-project dependencies.

PHP doesn't have the ability to handle extensions on a per-project basis<sup id="a0">[0](#f0)</sup>, however, so the corresponding tool for extensions — called [PECL](https://pecl.php.net/) — is still in common use.

PECL downloads and builds open source extensions from the PECL repository. For example, to get the `memcached` extension for (as you'd suspect) memcached support, you can do it with PECL with one line:

```sh
pecl install memcached
```

Behind the scenes, this downloads the source tarball, uses PHP's build system to build it, and then (if it's running as a user with permission to do so), installs the resulting `memcached.so` in the right directory for PHP to find it and adds an `extension=memcached.so` line to an INI file.

### Manual builds

It's also possible to download and build an extension by hand.<sup id="a1">[1](#f1)</sup> This is a five step process, once you have the source code:

1. Run `phpize`, which uses autotools and the `config.m4` within the source code to build a configure script.
2. Run `./configure`. If the extension depends on another library, then there will probably be a flag to tell it where to find the library. This will be familiar if you're used to building many GNU and Linux projects that use autotools.
3. Run `make` to build the extension. You can stop here if you don't want to install the extension (in that case, you can find the extension `.so` in `.libs`).
4. Run `make install` to install the extension into PHP's extensions directory.
5. Add an `extension=foo.so` line to a `.ini` file somewhere PHP can find it.

## Anatomy of an extension

The New Relic PHP agent isn't a great extension to try to start with when trying to understand how an extension is put together: it's overly complicated, and not terribly cleanly put together.

Instead, you should look at extensions in PHP's source code. They're contained within the [`ext`](https://github.com/php/php-src/tree/master/ext) directory, and some are quite easy to understand. An example of a nice, self-contained extension is the [`posix`](https://github.com/php/php-src/tree/master/ext/posix) extension, which provides a PHP API into a variety of standard POSIX C functions.

Rather than regurgitating what others have written better, here are two resources you should go at least skim and look at in conjunction with either `ext/posix`, or a skeletal extension that you build yourself:

1. Sara Golemon's [_Extending and Embedding PHP_](https://www.amazon.ca/Extending-Embedding-PHP-Sara-Golemon/dp/067232704X), which we have at least one copy of in the Portland office. Specifically, chapters 5 through 7 are key to understanding how an extension is put together. This book used to be the bible of writing PHP extensions, and even though it's now over ten years old and mostly deals with PHP 4.4 and 5.1, it's still surprisingly relevant today.
2. The [PHP Internals Book](http://www.phpinternalsbook.com/) is an ongoing, incomplete project to write an up to date guide to interacting with the Zend Engine. It has [a rough guide to the layout of PHP 7 extensions](http://www.phpinternalsbook.com/php7/extensions_design/extension_skeleton.html), which should be enough to figure out the differences from Sara's book.

## Footnotes

<sup id="f0">0</sup> This is why Docker has become _very_ popular with many PHP ops people very quickly: if you deploy each project in its own container, then you do effectively get per-project control over which extensions are loaded. [↩](#a0)

<sup id="f1">1</sup> If you're adventurous, you can actually do this with the PHP agent itself, provided you have a PHP install somewhere in your `$PATH`: build axiom first with `make axiom` from the top `php_agent` directory, then `cd agent` and run `phpize && ./configure && make && make install`. It works! [↩](#a1)
