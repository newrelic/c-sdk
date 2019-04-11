<?php

if (!extension_loaded('pdo')) {
  die("skip: pdo extension required\n");
}

if (!extension_loaded('pdo_sqlite')) {
  die("skip: pdo_sqlite extension required\n");
}
