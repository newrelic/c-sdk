<?php

/*DESCRIPTION
When capture_params is enabled, but a security token is set, no request parameters should be captured.
*/

/*INI
newrelic.capture_params = 1
newrelic.security_policies_token = "FFFFFFFFFFFFFFFF"
*/

/*ENVIRONMENT
REQUEST_METHOD=GET
QUERY_STRING=foo=1&bar=2
*/

/*EXPECT_TRACED_ERRORS
[
  "?? agent run id",
  [
    [
      "?? when",
      "WebTransaction/Uri__FILE__",
      "HACK: forced error",
      "NoticedError",
      {
        "stack_trace": "??",
        "agentAttributes": {
          "response.headers.contentType": "text/html",
          "httpResponseCode": "200",
          "SERVER_NAME": "??",
          "request.method": "GET",
          "request.headers.host": "??"
        },
        "intrinsics": "??",
        "request_uri": "??"
      }
    ]
  ]
]
*/

require_once(realpath (dirname ( __FILE__ )) . '/../../../include/helpers.php');

/*
 * Request parameters are not put into the transaction event when enabled by
 * non-attribute configuration, therefore an error or trace is required.
 */
force_error();
