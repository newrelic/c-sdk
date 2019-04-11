<?php

if (!extension_loaded("uopz")) {
  // Attempt to load uopz.
  $result = @dl('uopz.so');
  if (!$result) {
    die("skip: uopz extension required");
  }
}

// These tests are only for the PHP 7 version of uopz.
if (version_compare(PHP_VERSION, '7.0.0', '<')) {
  die("skip: requires PHP 7.0+\n");
}
