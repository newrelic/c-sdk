<?php

/*DESCRIPTION
newrelic_record_datastore_segment() should still execute the callback when
invalid parameters are provided.
*/

/*EXPECT
int(42)
int(2)
string(36) "Missing datastore parameter: product"
*/

@var_dump(newrelic_record_datastore_segment(function () {
  // Make sure this function takes at least 1 microsecond to ensure that a trace
  // node is generated.
  time_nanosleep(0, 1000);
  return 42;
}, array()));
$error = error_get_last();
var_dump($error['type'], $error['message']);
