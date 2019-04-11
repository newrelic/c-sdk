/*DESCRIPTION
This test ensures that calling ignore transactions will send no data.
*/

/*EXPECT_HARVEST
no
*/

#include "common.h"

RUN_NONWEB_TXN("basic") {
  newrelic_ignore_transaction(txn);
}
