<?php

/*DESCRIPTION
uopz_undefine
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*EXPECT
int(1)
int(0)
*/

class Foo {
	const BAR = 1;
}

var_dump(FOO::BAR);

uopz_undefine(Foo::class, "BAR");

$reflector = new ReflectionClass(Foo::class);

var_dump(count($reflector->getConstants()));
