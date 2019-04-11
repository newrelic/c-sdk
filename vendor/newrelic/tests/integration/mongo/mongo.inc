<?php

require_once(realpath (dirname ( __FILE__ )) . '/../../include/config.php');

function mongo_server()
{
  global $MONGO_HOST, $MONGO_PORT;

  return "mongodb://${MONGO_HOST}:${MONGO_PORT}";
}
