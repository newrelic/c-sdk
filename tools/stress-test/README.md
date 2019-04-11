# stress-test-validate-results

## Running the Stressor

We have an application written in C, buildable via this folder (`tools/stress-test`)

    make stress_app

Once you've compiled and started the daemon

    make daemon
    ./php_agent/bin/daemon -f --logfile stdout --loglevel debug

running this application

    ./stress_app 10 6 600

will send a certain number of transaction, per a certain number of seconds to the staging collector for the application named *C-Agent Test App*.  The above invocation will send 10 transactions every 6 seconds for 600 seconds (10 minutes).

The transaction will always be named `StressTransaction_1`, and the error will always have a type of `StressTransactionError`.

The application will report its progress as it sends transactions.

    It is now 2018-01-09 23:59:48
    I have sent 450 transactions/errors since: 2018-01-09 23:55:18
    I have been running for 270 seconds.

Once the application finishes it will report the total transactions sent, along with sample invocations for the *stress-test-validate-results* app.

## Checking the Values

Once the application finishes running, we'll have a start and end time.

    It is now 2018-01-10 00:05:13
    I have sent 990 transactions/errors since: 2018-01-09 23:55:18
    I have been running for 595 seconds.

    It is now 2018-01-10 00:05:19
    I finished running at 2018-01-10 00:05:19

There's a simple validation tool that lives in this folder (`tools/stress-test`).  It's a composer based PHP application.   Just run

    $ composer install

from `tools/stress-test-validate-results` to install its dependencies. (if you're not familiar with composer or on a system with a wonky version of PHP, see below for instructions on running *stress-test-validate-results* via Docker)

The validation application will fetch a count of the `StressTransaction_1` transactions with a `StressTransactionError` error.  Here's how to invoke it

    $ php test.php "2018-01-09 23:55:18" "2018-01-10 00:05:13"

    ErrorType              Count
    --------------------------------------------------
    StressTransactionError	990
    --------------------------------------------------
    Total Errors: 990

    Running NRQL:

    SELECT count(*)
    FROM Transaction
    FACET errorType
    WHERE appName='C-Agent Test App'
    AND errorType = 'StressTransactionError'
    AND name LIKE '%StressTransaction_1'
    SINCE '2018-01-09 23:55:18'
    UNTIL '2018-01-10 00:05:19'

    SELECT count(*)
    FROM TransactionError
    FACET `error.class`
    WHERE appName='C-Agent Test App'
    AND `error.class` = 'StressTransactionError'
    SINCE '2018-01-09 23:55:18'
    UNTIL '2018-01-10 00:05:19'

Once invoked, the application will report on the number of total Transaction events recorded with the error class, as well as the total number of TransactionError events with the same error class.  Compare these totals with the total from your run of `./stress_app` (Keeping in mind limits like 100 error events per transaction, etc)

Running Via Docker
--------------------------------------------------
If you're having trouble running the validation app via your system PHP, you can give the dockerized version a try.  First, you'll need to build the Docker image (the below image is tagged with the name `c-agent-error-validation` -- but feel free to use whatever name you'd like)

    docker build . -t c-agent-error-validation

Once you've built the image, you can run the docker container instead of running the program directly with your system php

    docker run -it c-agent-error-validation "2018-01-09 23:55:18" "2018-01-10 00:05:13"

You'll need to set two environmental variables to do anything useful -- you may want to *alias* the command if you plan to use it a lot

    docker run -it -e NEW_RELIC_INSIGHTS_ACCOUNT_ID=432507 \
                   -e NEW_RELIC_INSIGHTS_API_KEY=p9tGyebciUGLTN7r5MI_9nPpl9wsMg2E \
                   c-agent-error-validation "2018-01-09 23:55:18" "2018-01-10 00:05:13"

    ErrorType              Count
    --------------------------------------------------
    StressTransactionError	990
    --------------------------------------------------
    Total Errors: 990

    Running NRQL:

    SELECT count(*)
    FROM Transaction
    FACET errorType
    WHERE appName='C-Agent Test App'
    AND errorType = 'StressTransactionError'
    AND name LIKE '%StressTransaction_1'
    SINCE '2018-01-09 23:55:18'
    UNTIL '2018-01-10 00:05:13'

Testing Strategies
--------------------------------------------------
These tools aren't prescriptive, and don't provide a simple pass/fail.  They're intended to automated manual testing and allow a developer to quickly try out different traffic scenarios.

For example, here's one scenario that sends 500 transactions every 5 seconds for 15 seconds.

    ./stress_app 500 5 15

Which results in transaction counts of

    EventType          ErrorType                    Count
    -------------------------------------------------------
    Transaction        StressTransactionError	      1500
    TransactionError   StressTransactionError	      300
    -------------------------------------------------------
    WARNING: Transaction event count and TransactionError event count don't match.

These mismatched count seems like a problem, until you consider the limits on TransactionError events (``MaxErrorEvents = 100` from the Go Agent).  Rather than needing to gin up individual test applications, the `stress_app` allows developers to quickly try out scenarios and investigate results.
