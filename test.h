#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

#include "terminal_color.h"

#define FAIL_FAST 0

#define FLOAT_DIFFERENCE_TOLERANCE (0.00001)

#define SHOW_LOCATION() \
        do { fprintf(stderr, "%s: %s() line %d \n\n", __FILE__, \
                                 __func__, __LINE__); } while (0)

#define PASS_TEST(expr, name) do { printf(""   ANSI_COLOR_GREEN   "Test: " name " -- " #expr " -- PASS   "  ANSI_COLOR_RESET "\n"); } while(0)
#define FAIL_TEST(expr, name) do { printf("\n" ANSI_COLOR_RED     "Test: " name " -- " #expr " -- FAILED "  ANSI_COLOR_RESET "\n"); printf("** TEST FAILED ** \n"); } while(0)

// Test that the given expression is true
#define TEST(expr, name) \
do { \
if(expr == 1) { PASS_TEST(expr, name); } \
else { FAIL_TEST(expr, name); SHOW_LOCATION(); if(FAIL_FAST) exit(EXIT_FAILURE); }\
 } while(0)

#define SECTION(name) \
do { \
printf("\n\n  " ANSI_COLOR_BRIGHT_YELLOW "** " name " ** " ANSI_COLOR_RESET "\n\n\n");\
} while(0)

int floats_equal(float one, float two) {
  return abs( one - two ) < FLOAT_DIFFERENCE_TOLERANCE;
}

#endif
