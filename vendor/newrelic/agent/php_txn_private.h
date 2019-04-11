/*
 * Returns whether a particular security policy feature is considered
 * secure or not according to the current client configuration. These
 * values are not the ultimate source of truth for whether a certain
 * security policy is enabled or not.  The agent sends these values to the
 * daemon for further calculation/consideration.
 */
bool nr_php_txn_is_policy_secure(const char* policy_name,
                                 const nrtxnopt_t* opts);

/*
 * Returns an object of supported policies
 *
 * We need to send the daemon a hash of the LASP policies we support.
 * This function returns those policies as an nro object with the
 * follow structure
 *
 * {"policy_name":{         //the policy name
 *    "supported": bool     //does the agent support this policy
 *    "enabled: bool        //does the policy seem enabled or
 *                          //disabled from the pov of the configuration
 *
 * @return An nrobj_t*, caller is responsible for freeing with nro_delete
 */
nrobj_t* nr_php_txn_get_supported_security_policy_settings();
