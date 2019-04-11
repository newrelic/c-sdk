Suite: Most Secure
==================================================
The tests in this suite are intended to be be against the license key found in `nr-license.txt` and the security token in `security-token.txt`.  This is a dedicated testing account on staging (ID: 10002849) whose values should not change. The policy values for this account are as follows

    record_sql                      enabled:false
    attributes_include              enabled:false
    allow_raw_exception_messages    enabled:false
    custom_events                   enabled:false
    custom_parameters               enabled:false
    custom_instrumentation_editor   enabled:false
    message_parameters              enabled:false
    job_arguments                   enabled:false

These tests may be invoked with the integration runner with a make invocation (run from the root repository folder) that looks like this.

    make lasp-test SUITE_LASP=suite-most-secure
