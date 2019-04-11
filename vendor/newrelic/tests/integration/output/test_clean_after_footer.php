<?php

/*DESCRIPTION
No RUM content should be emitted if the output buffer is cleared after the
header and footer have been injected.
*/

// We need output_buffering to be a non-zero value, but the exact value doesn't
// matter, as nr_php_install_output_buffer_handler() hardcodes 40960 as the
// internal buffer size. 4096 has been chosen simply because it matches most
// default distro configurations.

/*INI
output_buffering = 4096
*/

/*ENVIRONMENT
REQUEST_METHOD=GET
*/

/*EXPECT
</html>
*/

?>
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="UTF-8">
  </head>
  <body>
  </body>
<?php ob_clean() ?>
</html>
