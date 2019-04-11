<?php

if (!extension_loaded("sqlite")) {
  die("skip: sqlite extension required");
}

if (!class_exists('SQLiteDatabase')) {
  die("skip: SQLiteDatabase class must exist");
}
