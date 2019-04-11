<?php

/*DESCRIPTION
uopz_redefine
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*EXPECT
int(1)
int(2)
*/

class Foo {
	const BAR = 1;
}

var_dump(FOO::BAR);

uopz_redefine(Foo::class, "BAR", 2);

var_dump(FOO::BAR);
