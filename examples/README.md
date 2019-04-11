# C Agent Instrumentation Examples

The C Agent is an software development kit that allows users to manually instrument applications written in C 
or C++ for Linux 64-bit operating systems. The source code in the `examples` directory provides examples of
how to instrument code.

All examples in this directory depend on two environment variables. These are:

- `NEW_RELIC_APP_NAME`. A meaningful application name.
- `NEW_RELIC_LICENSE_KEY`. A valid New Relic license key.

Set these environment variables before executing the example programs. Programs shall warn users if the
variables are unset.

The examples in this directory presume that the locations of libnewrelic.h and libnewrelic.a are one
level above this `examples` directory. If that is not the case, take care to set the following values
in `Makefile` appropriately:

```
LIBNEWRELIC_A_LOCATION = ..
LIBNEWRELIC_H_LOCATION = ../include
```

## Compiling and Executing Examples

Suppose you'd like to see an example of how `newrelic_notice_error()` works, to better understand how 
to record transaction errors.  The source file `ex_notice_error.c` offers such an example. 
To compile and execute this example, from the `examples` directory:

```
$ make ex_notice_error.out
$ ./ex_notice_error.out

```

After executing the example program, visit the New Relic Error Analytics dashboard for the account 
corresponding to the license key set as `NEW_RELIC_LICENSE_KEY`. For each time that `ex_notice_error.out`
is executed an error with the message "Meaningful error message" and error class "Error.class.supervalu"
is reported to New Relic and should be subsequently available at the aforementioned dashboard.

It's also possible to compile all the examples at once.  Again, from the `examples` directory:

```
$ make
```
