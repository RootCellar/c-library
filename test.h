#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

#include "terminal_color.h"
#include "debug.h"

#define FAIL_FAST 1

#define TEST_PASS_OUTPUT_COLOR ANSI_COLOR_BRIGHT_GREEN
#define TEST_FAIL_OUTPUT_COLOR ANSI_COLOR_BRIGHT_RED

#define FLOAT_DIFFERENCE_TOLERANCE (0.0001)

#define SHOW_LOCATION() \
        do { fprintf(stdout, "%s: %s() line %d \n\n", __FILE__, \
                                 __func__, __LINE__); } while (0)

#define PASS_TEST(expr, name) do { \
                                   printf(""   "%s Test: %s -- " #expr " -- PASS   " "%s\n", TEST_PASS_OUTPUT_COLOR, name, ANSI_COLOR_RESET); \
                                   } while(0)

#define FAIL_TEST(expr, name) do { printf("\n" "%s Test: %s -- " #expr " -- FAILED " "%s\n", TEST_FAIL_OUTPUT_COLOR, name, ANSI_COLOR_RESET); \
                                   printf("%s ** TEST FAILED ** " "%s\n", TEST_FAIL_OUTPUT_COLOR, ANSI_COLOR_RESET); SHOW_LOCATION(); \
                                   if(FAIL_FAST) exit(EXIT_FAILURE); \
                                   } while(0)

// Test that the given expression is true
#define __TEST(expr, name, quiet_pass) \
do { \
if((expr) == 1) { if(!quiet_pass) PASS_TEST(expr, name); } \
else { FAIL_TEST(expr, name); }\
 } while(0)

#define TEST(expr, name) __TEST(expr, name, 0)
#define TEST_QUIET(expr, name) __TEST(expr, name, 1)

#define SECTION(name) \
do { \
printf("\n\n  " "%s ** " name " ** %s" "\n\n\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);\
} while(0)

float float_abs(float value) {
  if(value < 0.0) return value * -1.0;
  return value;
}

int floats_equal(float one, float two) {
  // debug_printf(" %f == %f ? ", one, two);
  return float_abs(one - two) < FLOAT_DIFFERENCE_TOLERANCE;
}

#endif
