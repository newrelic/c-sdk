<?php

/*DESCRIPTION
The agent should not capture and report errors configured via the
newrelic.error_collector.ignore_errors setting.
*/

/*INI
error_reporting = E_ALL | E_STRICT
newrelic.error_collector.ignore_errors = E_WARNING
*/

/*EXPECT_REGEX
^\s*(PHP )?Warning:\s*Division by zero in .*? on line [0-9]+\s*$
*/

/*EXPECT_TRACED_ERRORS
null
*/

/*EXPECT_ERROR_EVENTS
null
*/

function run_test() {
  $x = 8 / 0;
}

run_test();
