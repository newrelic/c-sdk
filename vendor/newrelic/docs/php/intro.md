# What is PHP?

[PHP](https://secure.php.net/) is a dynamically, weakly<sup id="a0">[0](#f0)</sup> typed language that was created by [Rasmus Lerdorf](https://en.wikipedia.org/wiki/Rasmus_Lerdorf) in 1995. It was originally developed to provide a better alternative to using C, Perl, or Bourne shell scripts via CGI for writing backends for simple Web sites, and although it now functions as a general scripting language as well, it is still focused primarily on Web site backends.

An unusual feature of the language is that it is formatted as a templating language: code is only interpreted when contained within special tags (usually `<?php` and `?>`). Any other data within a PHP source file is output verbatim. This feels somewhat odd nowadays, but makes more sense in the time it was designed, as you could write, for example, a hit counter on a page with only a tiny piece of actual code<sup id="a1">[1](#f1)</sup>:

```php
<html>
  <head>
    <title>My awesome page</title>
  </head>
  <body>
    Insert lots of mid-90s animated GIFs here.
    <hr>
    You are visitor number
    <?php
    mysql_query('UPDATE hits SET counter = counter + 1');
    echo mysql_result(mysql_query('SELECT counter FROM hits'), 0);
    ?>!
  </body>
</html>
```

PHP also has an unusual execution model: unlike most Web backend languages which are stateful, all PHP requests start with a clean environment. This increases the processing overhead required to handle a request (as code has to be loaded and potentially parsed into bytecode if it's not cached, database connections have to be re-established, and session data has to be retrieved and parsed from a data store), but greatly decreases the cognitive overhead for the developer, as you control exactly what the state is and don't have to figure out implied state from previous requests.

These features made the barrier to entry incredibly low, and resulted in lots of people with no formal background in programming using PHP to develop their own sites.

It isn't hyperbolic to credit the explosion in dynamic Web sites around the late 90s and early 00s primarily to PHP.

## Evolution of the language

PHP started out as little more than a handful of functions in a templating language. As a language, its key inflection points were:

* 1998: Version 3.0 adds the Zend<sup id="a2">[2](#f2)</sup> VM, rather than interpreting source files line by line.
* 2000: Version 4.0 adds basic object orientation support along the same lines as Perl: objects are a thin wrapper over hashtables with methods.
* 2004: Version 5.0 rewrites the VM for increased performance and to support a more robust object model, which is now patterned after pre-generics Java.
* 2009: Version 5.3<sup id="a3">[3](#f3)</sup> adds support for namespaces<sup id="a4">[4](#f4)</sup>, closures, garbage collection<sup id="a5">[5](#f5)</sup>, and a lot more.
* 2015: Version 7.0 approximately doubles performance, rewrites the parser to allow for more expressive syntax, adds optional strong typing, and replaces most of the traditional PHP error subsystem with exceptions.

## Where is PHP used today?

Today, PHP is used for [approximately 80%](https://w3techs.com/technologies/overview/programming_language/all) of the Web sites in the world. That said, that is skewed significantly towards small blogs, brochure sites, and the like, and the amount of traffic on the Web that is actually served by PHP is considerably lower.

The biggest sites that use PHP (the language, at least — more on that in a moment) are Facebook, Yahoo, Wikipedia, Tumblr, and WordPress.com<sup id="a6">[6](#f6)</sup>.

## Implementations

Most users of PHP use the [mainline PHP implementation](https://secure.php.net). This is the only implementation of the language that the PHP agent supports, and although there is a [language specification](https://github.com/php/php-langspec), in practice the behaviour of PHP the language is basically "whatever mainline does".

The mainline implementation will be described in more detail in future chapters, but can be broadly understood as a traditional dynamic language in 2017: source code is compiled to bytecode at execution time, and then executed by a bespoke virtual machine (the Zend VM).

The only other implementation in remotely common usage as of 2017 is [HHVM](http://hhvm.com/), an implementation of PHP that uses a JIT to compile PHP code to native code. HHVM also supports [Hack](http://hacklang.org/), which is a superset of PHP that adds additional type checking, generics, and collections. HHVM and Hack are used by Facebook, but have seen relatively little adoption in the broader ecosystem: while HHVM once featured significantly better performance than mainline PHP, PHP 7 basically caught up, many of Hack's features have been rolled back into recent PHP versions, and HHVM's stability is often questionable when using features Facebook themselves don't use.

New Relic had a project to add support for HHVM to the PHP agent that went for about a year, but we ultimately cancelled it: the technical challenges were significant, and the customers that had requested it largely moved back to mainline PHP for other reasons.

## Footnotes

<sup id="f0">0</sup> Ish. Objects have always been strongly typed, and you should be aware of [type declarations](https://secure.php.net/manual/en/functions.arguments.php#functions.arguments.type-declaration), which — especially in PHP 7 where they're allowed to be used with scalar types — permit strong typing to used in almost all cases. [↩](#a0)

<sup id="f1">1</sup> This is intentionally terrible, circa 1998 PHP 3 code. Please never write code like this. Plus, the `mysql_*` functions don't work on PHP 7 anyway. [↩](#a1)

<sup id="f2">2</sup> Zend is a portmanteau of two early PHP contributors: [Zeev Suraski](https://en.wikipedia.org/wiki/Zeev_Suraski) and [Andi Gutmans](https://en.wikipedia.org/wiki/Andi_Gutmans). They wrote the first bytecode based VM for PHP in 1997-98, named it after themselves, and then set up a similarly named company called [Zend Technologies](http://www.zend.com/) — now owned by Rogue Wave — to commercialise value added technologies for PHP. [↩](#a2)

<sup id="f3">3</sup> In hindsight, version 5.3 should have been numbered 6.0, which is acknowledged by basically everyone involved. At the time, it wasn't because version 6.0 was being developed in parallel to add support for Unicode strings as a first class type (much in the same vein as Python 3). This work ended up failing — performance took too much of a hit, and with the increasing adoption of UTF-8, it became apparent that there was little demand for the additional theoretical safety 6.0 was to provide. The major version of PHP after 5 is numbered 7 to avoid confusion: multiple books were released that claimed to teach PHP 6, even though PHP 6 never even had an alpha release! [↩](#a3)

<sup id="f4">4</sup> Think C++ namespaces, right down to the internal implementation being based on name mangling rather than having namespaces be a first class type like Python modules. [↩](#a4)

<sup id="f5">5</sup> PHP 5.2 and earlier handled resource cleanup via pure reference counting, which obviously had issues with reference cycles (particularly once PHP 5 moved all objects to being passed by reference). This wasn't a major issue when PHP only handled short, simple Web requests, but was an increasing problem in a world where developers wanted to use PHP for long running daemons. [↩](#a5)

<sup id="f6">6</sup> As you'd expect, WordPress.com runs WordPress the framework. It's the backend for more sites than you likely realise: WordPress evolved beyond simple blogs many years ago, the ecosystem includes a surprising number of e-commerce sites, and WordPress.com is very good at getting out of the way when its customers don't want users to know WordPress is the backend. [↩](#a6)

