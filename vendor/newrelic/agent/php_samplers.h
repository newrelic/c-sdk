/*
 * This file contains periodic system samplers.
 */
#ifndef PHP_SAMPLERS_HDR
#define PHP_SAMPLERS_HDR

/*
 * Purpose : Initialize the system samplers.
 *
 * Params  : None.
 *
 * Returns : Nothing.
 */
extern void nr_php_initialize_samplers(void);

/*
 * Purpose : Sample system resources and store results so that system usage can
 *           later be properly calculated.
 *
 * Params  : None.
 *
 * Returns : Nothing.
 */
extern void nr_php_resource_usage_sampler_start(TSRMLS_D);

/*
 * Purpose : Sample system resources and add the results to the transaction's
 *           metric table.
 *
 * Params  : None.
 *
 * Returns : Nothing.
 */
extern void nr_php_resource_usage_sampler_end(TSRMLS_D);

#endif /* PHP_SAMPLERS_HDR */
