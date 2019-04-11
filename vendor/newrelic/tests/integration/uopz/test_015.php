<?php

/*DESCRIPTION
uopz_implement
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*EXPECT
bool(true)
string(54) "the class provided (Bar) already has the interface Foo"
*/

interface Foo {}
class Bar {}

uopz_implement(Bar::class, Foo::class);

$bar = new Bar;

var_dump($bar instanceof Foo);

try {
	uopz_implement(Bar::class, Foo::class);
} catch (Throwable $t) {
	var_dump($t->getMessage());
}
