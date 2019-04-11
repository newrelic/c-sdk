/*
 * This file contains functions relating to httprequest::send external calls.
 */
#ifndef PHP_HTTPREQUEST_SEND_HDR
#define PHP_HTTPREQUEST_SEND_HDR

/*
 * Purpose : Add cross process request headers to a HttpRequest instance
 *           before an httprequest::send call.
 */
extern void nr_php_httprequest_send_request_headers(zval* this_var TSRMLS_DC);

/*
 * Purpose : Get the cross process response header from an HttpRequest instance
 *           after an httprequest::send call.
 */
extern char* nr_php_httprequest_send_response_header(zval* this_var TSRMLS_DC);

/*
 * Purpose : Get the URL from an HttpRequest instance.
 */
extern char* nr_php_httprequest_send_get_url(zval* this_var TSRMLS_DC);

#endif /* PHP_HTTPREQUEST_SEND_HDR */
