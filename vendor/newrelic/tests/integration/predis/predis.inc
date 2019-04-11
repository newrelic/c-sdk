<?php

$PREDIS_HOME = getenv("PREDIS_HOME");

if (empty($PREDIS_HOME)) {
  die("skip: PREDIS_HOME not defined\n");
}

if (!file_exists($PREDIS_HOME)) {
  die("skip: PREDIS_HOME not found: ${PREDIS_HOME}\n");
}

$PREDIS_HOME = realpath($PREDIS_HOME);

if (file_exists($PREDIS_HOME . '/vendor/autoload.php')) {
  // this is a Composer installation
  require_once($PREDIS_HOME . '/vendor/autoload.php');
} else if (file_exists($PREDIS_HOME . '/autoload.php')) {
  // this is a non-Composer installation (maybe a git clone?)
  require_once($PREDIS_HOME . '/autoload.php');
} else {
  die("skip: autoload.php not found\n");
}

if (!class_exists('Predis\\Client')) {
    die("skip: Predis\Client not defined\n");
}
