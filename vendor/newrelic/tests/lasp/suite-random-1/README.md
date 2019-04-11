Suite: Random 1
==================================================
The tests in this suite are intended to be be against the license key found in `nr-license.txt` and the security token in `security-token.txt`.  This is a dedicated testing account (ID: 10003435) whose values should not change.  The policy values for this account are as follows

    record_sql                      enabled:true
    attributes_include              enabled:true
    allow_raw_exception_messages    enabled:false
    custom_events                   enabled:true
    custom_parameters               enabled:true
    custom_instrumentation_editor   enabled:true
    message_parameters              enabled:false
    job_arguments                   enabled:true

These tests may be invoked with the integration runner with a make invocation (run from the root repository folder) that looks like this.

    make lasp-test SUITE_LASP=suite-random-1
