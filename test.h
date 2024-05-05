#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

#define FAIL_FAST 0

#define SHOW_LOCATION() \
        do { fprintf(stderr, "%s: %s() line %d \n\n", __FILE__, \
                                 __func__, __LINE__); } while (0)

#define PASS_TEST(expr, name) do { printf(" Test: " name  " -- " #expr " -- Passed \n"); } while(0)
#define FAIL_TEST(expr, name) do { printf("\n Test: " name " -- " #expr " -- FAILED \n"); printf("** TEST FAILED ** \n"); } while(0)

// Test that the given expression is true
#define TEST(expr, name) \
do { \
if(expr == 1) { PASS_TEST(expr, name); } \
else { FAIL_TEST(expr, name); SHOW_LOCATION(); if(FAIL_FAST) exit(EXIT_FAILURE); }\
 } while(0)

#define SECTION(name) \
do { \
printf(" \n\n ** " name " ** \n\n\n ");\
} while(0)

#endif
