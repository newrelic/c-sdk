# Zend Engine concepts

This chapter can't possibly replace a guide like _Extending and Embedding PHP_ or the [PHP Internals Book](http://www.phpinternalsbook.com/), but here is a quick survey of the key types and ideas that you'll need to know to work on the PHP agent.

## Executors

Each request is handled by an executor: effectively, an instance of the Zend VM that runs whatever code it's given. Executors maintain state in so called "executor globals", which are instances of the [`zend_executor_globals` struct](https://php-lxr.adamharvey.name/source/search?q=&defs=_zend_executor_globals&refs=&path=&hist=&type=&project=PHP-5.2&project=PHP-5.3&project=PHP-5.4&project=PHP-5.5&project=PHP-5.6&project=PHP-7.0&project=PHP-7.1&project=PHP-7.2&project=PHP-7.3&project=master) that encapsulate the current state of the executor. Ultimately, all variables and functions within the current request scope can be accessed in some way through those fields.

Accessing this structure is usually done through the `EG()` macro, which takes the field name as its only parameter. For instance, to access the `active_op_array` field, you would use `EG(active_op_array)`. In the normal case, this is simply syntactic sugar for `executor_globals.active_op_array`, but if PHP is compiled with thread safety on, it gets more complicated.

## Thread safety

PHP can be compiled in thread safe (ZTS) or non thread safe (NTS) mode. Default builds are non thread safe, and in practice, this is used by >99% of users on non-Windows platforms (including _all_ distribution packages), and an increasingly large majority of Windows users<sup id="a0">[0](#f0)</sup>.

In NTS mode, globals are implemented as true process globals, but in ZTS mode they are accessed through a component called TSRM: the Thread-Safe Resource Manager. This is bundled with and integrated deeply into PHP, and provides an abstraction layer above OS-specific threading primitives to manage locking shared resources.

In PHP 5, this manifests primarily as a pointer that has to be passed around from function to function to handle the current TSRM state: this is commonly handled as a `void ***tsrm_ls` argument at the end of the C function's argument list. Any function that needs to access a global (either directly or implicitly) must receive this pointer and pass it the functions it calls that require that pointer.

Because this is tedious, a set of macros are available within the PHP source code to handle this:

* `TSRMLS_DC`: **D**eclares a `tsrm_ls` argument if ZTS mode is enabled, including adding a **c**omma (so that you don't have a trailing comma in the prototype). For example, declaring a function as `void foo(int bar TSRMLS_DC)` results in one of these prototypes:
  * ZTS: `void foo(int bar, void ***tsrm_ls)`
  * NTS: `void foo(int bar)`

* `TSRMLS_D`: The same, but without the comma, for functions that otherwise take no arguments.

* `TSRMLS_CC`: Passes the `tsrm_ls` pointer within the current scope to the function being **c**alled with a **c**omma. For example, calling a function with `foo(42 TSRMLS_CC)` results in:
  * ZTS: `foo(42, tsrm_ls)`
  * NTS: `foo(42)`

* `TSRMLS_C`: The same, but without the comma, for functions that otherwise take no arguments.

* `TSRMLS_FETCH`: In very rare cases, you may not have a `tsrm_ls` pointer given to you, but you need to access it. This macro expands to `void ***tsrm_ls = (void ***) ts_resource_ex(0, NULL);`, which gets it from the OS thread local storage.

    In practice, this is usually a code smell, but there are a handful of legitimate cases where PHP's API doesn't allow easy access to `tsrm_ls`.

On PHP 7, all accesses are handled implicitly through better use of each OS's native threading API without needing an explicit state variable. Nevertheless, the aforementioned macros are still defined (and just always expand to nothing) so that code can be kept compatible with both versions of PHP.

## `zval`

A [`zval`](https://php-lxr.adamharvey.name/source/search?q=&defs=_zval_struct&refs=&path=&hist=&type=&project=PHP-5.2&project=PHP-5.3&project=PHP-5.4&project=PHP-5.5&project=PHP-5.6&project=PHP-7.0&project=PHP-7.1&project=PHP-7.2&project=PHP-7.3&project=master) is a C structure that represents a PHP variable. It contains type information, the value of the variable in a union, and for garbage collected types (which is all of them in PHP 5, and non-scalar types in PHP 7), reference counting information.

On PHP 5, a `zval` is always passed as a pointer; PHP 7 is a bit murkier (at least internally). In the PHP agent we always use them as pointers for consistency.

`zval`s are generally accessed and manipulated through a set of macros. Rather than listing them here, you should either look at code that deals with the type(s) you're interested in, or read chapter 2 of _Extending and Embedding PHP_. (If you do the latter, be aware that strings in particular have new macros in PHP 7 that you'll need to know about.)

It's crucial to note that you can't get or set the value of a `zval` without knowing the type. Using the raw Zend Engine API, getting the value of an integer defensively looks something like this:

```c
zval *zv;

// ...

if (Z_TYPE_P(zv) == IS_LONG) {
  return Z_LVAL_P(zv);
}
```

Note that we used `Z_TYPE_P()` to check the type (the `_P` suffix indicates that the argument is a `zval *`; there are also unsuffixed versions that take a straight `zval` and `_PP` versions that take a `zval **`) before using `Z_LVAL_P()` to get the actual integer value.

## `zend_execute_data`

PHP's call stack is implemented internally as a linked list of [execute data frames](https://php-lxr.adamharvey.name/source/search?q=&defs=_zend_execute_data&refs=&path=&hist=&type=&project=PHP-5.2&project=PHP-5.3&project=PHP-5.4&project=PHP-5.5&project=PHP-5.6&project=PHP-7.0&project=PHP-7.1&project=PHP-7.2&project=PHP-7.3&project=master). Each frame has pointers linking it back to the function or file that is being run, fields representing the current state of the executor, and `zval`s representing the given arguments<sup id="a1">[1](#f1)</sup> and return value<sup id="a2">[2](#f2)</sup>.

The current execute data frame is available from the executor globals via `EG(current_execute_data)`.

## `zend_function`

Both internal and user functions are represented within PHP as [`zend_function` unions](https://php-lxr.adamharvey.name/source/search?q=&defs=_zend_function&refs=&path=&hist=&type=&project=PHP-5.2&project=PHP-5.3&project=PHP-5.4&project=PHP-5.5&project=PHP-5.6&project=PHP-7.0&project=PHP-7.1&project=PHP-7.2&project=PHP-7.3&project=master). Instances of this union will either be `zend_internal_function` or `zend_op_array` structs: these structs include a common set of fields at the start to encapsulate common concerns such as the function type, name, scope, and prototype.

## `zend_op_array`

The [`zend_op_array` struct](https://php-lxr.adamharvey.name/source/search?q=&defs=_zend_op_array&refs=&path=&hist=&type=&project=PHP-5.2&project=PHP-5.3&project=PHP-5.4&project=PHP-5.5&project=PHP-5.6&project=PHP-7.0&project=PHP-7.1&project=PHP-7.2&project=PHP-7.3&project=master) is used to represent a user function. The most useful aspect of this to note is that the actual opcodes to be executed by the Zend Engine are stored within this array as the `opcodes` array field.

This struct also includes a `reserved` array, which can be used by third party extensions to associate metadata with user functions. The PHP agent uses this to store wrap records for functions we're interested in.

## Footnotes

<sup id="f0">0</sup> The reason thread safe builds are more common on Windows is ultimately due to how Windows handles multi-processing: because Windows has much lower overhead when spawning threads than processes, IIS uses threading by default to implement server pools, and when PHP first got Windows support, ISAPI was the only way to implement PHP as an IIS module, which meant that PHP had to be thread safe itself.

In the many years since, ISAPI has fallen largely out of favour with third party modules, and PHP users wishing to use IIS are now strongly recommended to use a PHP-FPM pool via FastCGI à la nginx or Apache, in which case a non thread safe version of PHP can be used — as NTS builds are generally faster and more stable, this is considered a win-win. Nevertheless, ZTS builds for Windows are still in reasonably common use, particularly in legacy environments.

Of course, since the New Relic PHP agent doesn't support Windows, this isn't a major issue for us. [↩](#a0)

<sup id="f1">1</sup> On PHP 7, the arguments are usually packed into the memory immediately after the execute data struct, rather than being accessed via pointers within the execute data field. Doing so provided a significant performance boost due to cache locality. In practice, this just means that one needs to be careful to access function arguments using the appropriate macros and functions rather than directly. [↩](#a1)

<sup id="f2">2</sup> For simplicity, all PHP functions return a value in all cases: while PHP 7.1 added `void` as a return type, that is simply shorthand for the function always returning `null`. [↩](#a2)
