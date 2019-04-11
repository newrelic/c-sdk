<?php

/*DESCRIPTION
The agent should not report traced errors for uncaught exceptions that have been
blacklisted by the user.
*/

/*INI
newrelic.error_collector.ignore_exceptions = foo,bar,Exception,baz
*/

/*EXPECT_TRACED_ERRORS null */

/*EXPECT_ERROR_EVENTS null */

function alpha() {
  throw new Exception('Sample Exception');
}

function beta() {
  alpha();
}

function gamma($password) {
  beta();
}

gamma('my super secret password that New Relic cannot know');
