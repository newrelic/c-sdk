<?php

/*DESCRIPTION
uopz.disabled
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*INI
uopz.disable=1
*/

/*EXPECT_SCRUBBED
Fatal error: Uncaught RuntimeException: uopz is disabled by configuration (uopz.disable) in __FILE__:??
Stack trace:
#0 __FILE__(??): uopz_set_return()
#1 {main}
  thrown in __FILE__ on line ??
*/

uopz_set_return();
