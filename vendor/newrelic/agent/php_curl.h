/*
 * This file contains functions relating to external calls made using curl.
 */
#ifndef PHP_CURL_HDR
#define PHP_CURL_HDR

/*
 * Purpose : This function is added to the extension to provide a default
 *           curl response header callback.
 */
extern PHP_FUNCTION(newrelic_curl_header_callback);

/*
 * Purpose : Modify a newly created curl resource to support cross process
 *           headers.
 *
 * Params  : 1. A curl resource which has just been created by curl_init.
 */
extern void nr_php_curl_init(zval* curlres TSRMLS_DC);

/*
 * Purpose : Handle anything that should be done for curl external
 *           instrumentation before the original curl_setopt() handler is
 *           invoked. In practice, this means modifying callback parameters
 *           given to curl_setopt() to support cross process headers.
 *
 * Params  : The three parameters to curl_setopt: The curl resource, the option
 *           constant, and the value being set.
 */
extern void nr_php_curl_setopt_pre(zval* curlres,
                                   zval* curlopt,
                                   zval* curlval TSRMLS_DC);

/*
 * Purpose : Handle anything that should be done for curl external
 *           instrumentation after the original curl_setopt() handler is
 *           invoked. In practice, this means re-calling curl_setopt() if the
 *           user was setting headers via CURLOPT_HTTPHEADER.
 *
 * Params  : The three parameters to curl_setopt: The curl resource, the option
 *           constant, and the value being set.
 */
extern void nr_php_curl_setopt_post(zval* curlres,
                                    zval* curlopt,
                                    zval* curlval TSRMLS_DC);

typedef void (*nr_php_curl_setopt_func_t)(zval* curlres,
                                          zval* curlopt,
                                          zval* curlval TSRMLS_DC);

/*
 * Purpose : Handle anything that should be done for curl external
 *           instrumentation when curl_setopt_array() is called.
 *
 * Params  : 1. The curl resource.
 *           2. The options array.
 *           3. The function to invoke for each option: in practice, either
 *              nr_php_curl_setopt_pre or nr_php_curl_setopt_post.
 */
extern void nr_php_curl_setopt_array(zval* curlres,
                                     zval* options,
                                     nr_php_curl_setopt_func_t func TSRMLS_DC);

/*
 * Purpose : Get the url of a curl resource.
 *
 * Returns : A newly allocated string containing the url, or 0 on error.
 */
extern char* nr_php_curl_get_url(zval* curlres TSRMLS_DC);

/*
 * Purpose : Determines whether the url for a curl resource represents a
 *           protocol that should be instrumented by the agent.
 *
 * Params  : 1. A null-terminated string representing the url to test.
 *
 * Returns : Non-zero if the protocol should be instrumented; otherwise, zero.
 */
extern int nr_php_curl_should_instrument_proto(const char* url);

/*
 * Purpose : Add the New Relic headers to the request. If the user added headers
 * using curl_setopt they will have been save in curl_headers and we be added as
 * well.
 *
 * Params : Curl Resource
 */
extern void nr_php_curl_exec_set_httpheaders(zval* curlres TSRMLS_DC);

extern char* nr_php_curl_exec_get_method(zval* curlres TSRMLS_DC);

#endif /* PHP_CURL_HDR */
