<?php

// Pull in the newrelic/integration library. Since it normally uses an
// autoloader, we have to just include everything. Shitty, but it works.
//
// TODO(aharvey): consider copying in a basic PSR-4 autoloader and just
// registering it here.

if (!file_exists(__DIR__.'/newrelic-integration/composer.json')) {
  die("Run git submodule init && git submodule update to get the newrelic-integration library\n");
}

$files = array(
  'DatastoreInstance.php',
  'Metric.php',
  'SlowSQL.php',
  'Trace/Segment.php',
  'Trace/FilterIterator.php',
  'Trace/StringTable.php',
  'Trace.php',
  'Transaction.php',
);

foreach ($files as $file) {
  require_once __DIR__.'/newrelic-integration/src/'.$file;
}
