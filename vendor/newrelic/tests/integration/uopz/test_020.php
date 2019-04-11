<?php

/*DESCRIPTION
uopz_get_exit_status
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*INI
opcache.enable_cli=0
xdebug.enable=0
*/

/*EXPECT
int(10)
*/

exit(10);

var_dump(uopz_get_exit_status());
