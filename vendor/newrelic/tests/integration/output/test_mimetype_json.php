<?php

/*DESCRIPTION
PHP-579: The agent should use the current Content-Type header to determine
whether it is safe to perform RUM injection.
*/

/*ENVIRONMENT
REQUEST_METHOD=GET
*/

/*EXPECT
{"articlebody":"<body>Leave me alone!</body>"}
*/

header('Content-Type: text/html');
header('Content-Type: application/json');

?>
{"articlebody":"<body>Leave me alone!</body>"}
