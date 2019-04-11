<?php

/*DESCRIPTION
newrelic_ignore_transaction should not record a transaction, resulting in no harvest
*/

/*EXPECT_HARVEST no */

newrelic_ignore_transaction();
