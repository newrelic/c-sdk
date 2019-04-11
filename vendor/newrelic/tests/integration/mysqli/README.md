# Agent Integration Testing:  mysqli

A collection of procedures to exercise mysqli.

Written hastily by somebody who only read the manual and was looking
to increase coverage of the functions in php_instrument.c

Defaults to using values that work for a nrlamp mysql, with the usual
credentials.

## Configuration

Connection can be configured via environment variables:

  * MYSQL_HOST (default: "127.0.0.1")
  * MYSQL_PORT (default: 3306)
  * MYSQL_USER (default: "root")
  * MYSQL_PASSWD (default: "root")
  * MYSQL_DB (default: "information_schema")

## Notes
