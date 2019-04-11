<?php

/*DESCRIPTION
The agent should safely handle arguments that are not valid curl handles.
*/

/*SKIPIF
<?php
if (!extension_loaded("curl")) {
  die("skip: curl extension required");
}
*/

/*EXPECT_METRICS
[
  "?? agent run id",
  "?? start time",
  "?? stop time",
  [
    [{"name":"Errors/all"},                                      [1, "??", "??", "??", "??", "??"]],
    [{"name":"Errors/allOther"},                                 [1, "??", "??", "??", "??", "??"]],
    [{"name":"Errors/OtherTransaction/php__FILE__"},             [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/all"},                            [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransaction/php__FILE__"},                    [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime"},                       [1, "??", "??", "??", "??", "??"]],
    [{"name":"OtherTransactionTotalTime/php__FILE__"},           [1, "??", "??", "??", "??", "??"]]
  ]
]
*/

function test_curl()
{
    global $EXTERNAL_HOST;

    $ch = curl_init();

    curl_setopt($ch, CURLOPT_URL, $EXTERNAL_HOST);
    curl_setopt($ch, CURLOPT_NOBODY, true);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

    // provoke a type mismatch
    curl_exec("");
    curl_exec(false);
    // TODO(msl): try passing a resource that is not a curl handle

    curl_close($ch);
}

test_curl();
