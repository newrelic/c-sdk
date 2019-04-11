<?php

/*DESCRIPTION
The agent should not crash when an internal function is used as an exception
handler.
*/

/*EXPECT
Throwing...
*/

/*EXPECT_ERROR_EVENTS
null
*/

set_exception_handler('strlen');

function throw_it() {
  throw new RuntimeException('Hi!');
}

echo "Throwing...\n";
throw_it();
