Suite: Least Secure
==================================================
The tests in this suite are intended to be be against the license key found in `nr-license.txt` and the security token in `security-token.txt`. This is a dedicated testing account on staging (ID: 10005914) whose values should not change. The policy values for this account are as follows

    record_sql                      enabled:true
    attributes_include              enabled:true
    allow_raw_exception_messages    enabled:true
    custom_events                   enabled:true
    custom_parameters               enabled:true
    custom_instrumentation_editor   enabled:true
    message_parameters              enabled:true
    job_arguments                   enabled:true

These tests may be invoked with the integration runner with a make invocation (run from the root repository folder) that looks like this.

    make lasp-test SUITE_LASP=suite-least-secure
