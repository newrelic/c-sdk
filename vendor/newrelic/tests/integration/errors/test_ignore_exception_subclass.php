<?php

/*DESCRIPTION
The agent should also ignore subclasses of the classes listed in the
newrelic.error_collector.ignore_exceptions setting.
*/

/*INI
newrelic.error_collector.ignore_exceptions = Exception
*/

/*EXPECT_TRACED_ERRORS null */

/*EXPECT_ERROR_EVENTS null */

class MyException extends Exception
{
}

function alpha() {
  throw new MyException('Sample Exception');
}

function beta() {
  alpha();
}

function gamma($password) {
  beta();
}

gamma('my super secret password that New Relic cannot know');
