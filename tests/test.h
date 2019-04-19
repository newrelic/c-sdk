#ifndef TEST_H
#define TEST_H

#if defined(__GNUC__)
#define NRUNUSED __attribute__((__unused__))
#else
#define NRUNUSED
#endif

/*! @brief Provides a fake app as the group state. */
extern int app_group_setup(void** state);

/*! @brief Provides a fake transaction as the group state. */
extern int txn_group_setup(void** state);

/*! @brief Cleans up the fake app provided in the group state. */
extern int app_group_teardown(void** state);

/*! @brief Cleans up the fake transaction provided in the group state. */
extern int txn_group_teardown(void** state);

#endif
