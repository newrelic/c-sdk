<?php

/*DESCRIPTION
The agent should not capture and report runtime notices.
*/

/*INI
error_reporting = E_ALL | E_STRICT
*/

/*EXPECT_REGEX
^\s*(PHP )?Notice:\s*Undefined variable: usernmae in .*? on line [0-9]+\s*$
*/

/*EXPECT_TRACED_ERRORS
null
*/

/*EXPECT_ERROR_EVENTS
null
*/

function provoke_notice() {
  $username = 'foo';

  // Misspell username to cause a notice.
  if ($usernmae) {
    return 1;
  }
  return 0;
}

provoke_notice();
