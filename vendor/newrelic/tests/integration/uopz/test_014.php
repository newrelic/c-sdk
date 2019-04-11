<?php

/*DESCRIPTION
uopz_extend
*/

/*SKIPIF
<?php include("skipif.inc") ?>
*/

/*EXPECT
bool(true)
string(44) "the class provided (Foo) already extends Bar"
*/

class Foo {}
class Bar {}

uopz_extend(Foo::class, Bar::class);

$foo = new Foo;

var_dump($foo instanceof Bar);

try {
	uopz_extend(Foo::class, Bar::class);
} catch (Throwable $t) {
	var_dump($t->getMessage());
}
