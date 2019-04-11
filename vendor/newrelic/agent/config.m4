dnl This is the configuration file for the PHP build system, which is invoked
dnl by running phpize. This will then call autoconf and automake with the right
dnl magic invocations, and a configure script is generated.
dnl
dnl There is far too much complexity and subtlety to cover here. For an easier
dnl introduction to the various M4 macros that are used, you may want to refer
dnl to Sara Golemon's now somewhat outdated book "Extending and Embedding PHP",
dnl as the build system hasn't changed much down the years. You can also
dnl consult acinclude.m4 in php-src, as that includes a considerable amount of
dnl documentation on the PHP specific macros.

dnl First off, we define the configure switches we want to provide.

dnl This is basically unnecessary, since it's on by default and there's no
dnl situation where we'd build this in a php-src tree, but it is traditional,
dnl and PHP extension build scripts are nothing if not cargo cult exemplars.
PHP_ARG_ENABLE(newrelic, whether to enable New Relic RPM monitoring,
[  --enable-newrelic       Enable New Relic RPM monitoring],yes)

dnl The path to a libaxiom.a file. The default is to look for an inline library
dnl file, as built by running "make" in the axiom directory.
PHP_ARG_WITH(axiom,,
[  --with-axiom=DIR        Path to axiom], ../axiom, no)

if test "$PHP_NEWRELIC" = "yes"; then
  AC_DEFINE(HAVE_NEWRELIC, 1, [Whether you have New Relic])

  LT_AC_PROG_SED

  NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -std=gnu99"

  dnl Check headers.
  AC_CHECK_HEADER(malloc.h, [AC_DEFINE([HAVE_MALLOC_H], [1], [Have malloc.h])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_MALLOC_H=1"])
  AC_CHECK_HEADER(malloc/malloc.h, [AC_DEFINE([HAVE_MALLOC_MALLOC_H], [1], [Have malloc/malloc.h])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_MALLOC_MALLOC_H=1"])
  AC_CHECK_HEADER(sys/resource.h, [AC_DEFINE([HAVE_SYS_RESOURCE_H], [1], [Have sys/resource.h])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_SYS_RESOURCE_H=1"])

  dnl Check functions.
  AC_CHECK_FUNC(mstats, [AC_DEFINE([HAVE_MSTATS], [1], [Have the mstats() function])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_MSTATS=1"])
  AC_CHECK_FUNC(getrusage, [AC_DEFINE([HAVE_GETRUSAGE], [1], [Have the getrusage() function])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_GETRUSAGE=1"])
  AC_CHECK_FUNC(getpagesize, [AC_DEFINE([HAVE_GETPAGESIZE], [1], [Have the getpagesize() function])
    NEWRELIC_CFLAGS="$NEWRELIC_CFLAGS -DHAVE_GETPAGESIZE=1"])

  if test -f /proc/self/statm ; then
    AC_DEFINE(HAVE_PROC_SELF_STATM, 1, [Whether you have /proc/self/statm])
  fi

  dnl Set the appropriate system specific defines.
  case "$host" in

    *linux*)    AC_DEFINE(NR_SYSTEM_LINUX, 1, [Linux-based system])
                nrsys="linux"
                nrpic="-pic"
                LDFLAGS="$LDFLAGS -pthread -lm -ldl -lrt"
                EXTRA_LDFLAGS="$EXTRA_LDFLAGS -rdynamic -static-libgcc -export-symbols export.syms -Wl,-Map,newrelic.map"
                ;;

    *darwin*)   AC_DEFINE(NR_SYSTEM_DARWIN, 1, [Darwin-based system])
                nrsys="darwin"
                EXTRA_LDFLAGS="$EXTRA_LDFLAGS -pthread -Wl,-flat_namespace -static-libgcc -export-symbols export.syms -Wl,-map,newrelic.map"
                ;;

    *solaris*)  AC_DEFINE(NR_SYSTEM_SOLARIS, 1, [Solaris-based system])
                nrsys="solaris"
                nrpic="-pic"
                CFLAGS="$CFLAGS -D_POSIX_PTHREAD_SEMANTICS"
                LDFLAGS="$LDFLAGS -pthread -lmalloc -lm -lsocket -lnsl -lrt -static-libgcc"
                EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wc,-static-libgcc -export-symbols export.syms"
                ;;

    *freebsd*)  AC_DEFINE(NR_SYSTEM_FREEBSD, 1, [FreeBSD-based system])
                nrsys="freebsd"
                nrpic="-pic"
                LDFLAGS="$LDFLAGS -pthread -lm"
                EXTRA_LDFLAGS="$EXTRA_LDFLAGS -static-libgcc -export-symbols export.syms"
                ;;

  esac

  dnl TODO(aharvey): consider whether this is really necessary. We could
  dnl probably set semi-sensible defaults.
  if test -z "${nrsys}"; then
    AC_MSG_ERROR([unknown or unsupported system])
  fi

  dnl Our one external dependency is libpcre, which axiom needs. We'll use
  dnl pcre-config to find it, since every modern version of PCRE provides it.
  PCRE_INCLINE=`pcre-config --cflags`
  if pcre-config --prefix | grep -q /opt/nr/camp; then
    dnl nrcamp links the position independent libpcre into a different file
    dnl because... well, I have no idea, honestly.
    PCRE_LIBLINE=-lnrpcre-pic
    PCRE_LIBRARY=nrpcre-pic
  else
    PCRE_LIBLINE=`pcre-config --libs`
    PCRE_LIBRARY=pcre
  fi

  PHP_CHECK_LIBRARY($PCRE_LIBRARY, pcre_exec, [
    PHP_EVAL_INCLINE($PCRE_INCLINE)
    PHP_EVAL_LIBLINE($PCRE_LIBLINE, NEWRELIC_SHARED_LIBADD)
  ],[
    AC_MSG_ERROR([PCRE not found])
  ],[
    $PCRE_LIBLINE
  ])

  dnl Check for axiom.
  PHP_CHECK_LIBRARY(axiom, nro_new, [
    PHP_ADD_INCLUDE($PHP_AXIOM)
    dnl Avoid using PHP_ADD_LIBRARY and friends. They add an RPATH for
    dnl the axiom directory, and there's no way to prevent it.
    NEWRELIC_SHARED_LIBADD="-L$PHP_AXIOM -laxiom $NEWRELIC_SHARED_LIBADD"
  ],[
    AC_MSG_ERROR([axiom not found])
  ],[
    -L$PHP_AXIOM
  ])

  dnl Add the shared library information we just built up to the Makefile.
  PHP_SUBST(NEWRELIC_SHARED_LIBADD)

  dnl Also add the location of axiom to the Makefile for dependency tracking
  dnl purposes.
  PHP_SUBST(PHP_AXIOM)

  dnl We have various flags that we only use with gcc or clang (which is also
  dnl detected as the "GNU compiler"). We don't support any other compilers at
  dnl this time, but if and when we do, we'll probably want to use the
  dnl equivalent switches for them.
  if test "x$ac_cv_c_compiler_gnu" = "xyes"; then
    dnl Enable a set of warnings that are useful for code quality.
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -W"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wall"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wextra"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wswitch-enum"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wmissing-declarations"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wshadow"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wpointer-arith"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wcast-qual"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Werror"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wno-typedef-redefinition"
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -Wno-missing-field-initializers"

    dnl Enable building dependency files.
    NEWRELIC_CFLAGS="${NEWRELIC_CFLAGS} -MMD -MP"
  fi

  dnl Actually define our extension, and far more importantly, its source
  dnl files.
  PHP_NEW_EXTENSION(newrelic, fw_cakephp.c fw_codeigniter.c fw_drupal8.c fw_drupal.c fw_drupal_common.c fw_joomla.c fw_kohana.c fw_laravel.c fw_laravel_queue.c fw_magento1.c fw_magento2.c fw_magento_common.c fw_mediawiki.c fw_misc.c fw_silex.c fw_slim.c fw_support.c fw_symfony2.c fw_symfony.c fw_wordpress.c fw_yii.c fw_zend2.c fw_zend.c lib_doctrine2.c lib_guzzle3.c lib_guzzle4.c lib_guzzle6.c lib_guzzle_common.c lib_mongodb.c lib_phpunit.c lib_predis.c lib_zend_http.c php_agent.c php_api.c php_api_datastore.c php_api_distributed_trace.c php_api_internal.c php_autorum.c php_call.c php_curl.c php_datastore.c php_environment.c php_error.c php_execute.c php_explain.c php_explain_mysqli.c php_explain_pdo_mysql.c php_extension.c php_file_get_contents.c php_globals.c php_hash.c php_header.c php_httprequest_send.c php_internal_instrument.c php_minit.c php_mshutdown.c php_mysql.c php_mysqli.c php_newrelic.c php_nrini.c php_output.c php_pdo.c php_pdo_mysql.c php_pdo_pgsql.c php_pgsql.c php_psr7.c php_redis.c php_rinit.c php_rshutdown.c php_samplers.c php_stack.c php_txn.c php_user_instrument.c php_vm.c php_wrapper.c, $ext_shared,, \\$(NEWRELIC_CFLAGS))

  PHP_SUBST(NEWRELIC_CFLAGS)

  dnl Define $(PHP_CONFIG) so we can call it when building tests.
  PHP_SUBST(PHP_CONFIG)

  dnl Include the Makefile.frag, which we use to handle build time
  dnl dependencies.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
