<?php

if (!extension_loaded('mongo')) {
  die('skip: mongo extension required');
}

require('mongo.inc');

try {
  $client = new MongoClient(mongo_server());
  $client->close();
} catch (MongoConnectionException $e) {
  die('skip: ' . $e->getMessage() . "\n");
}
