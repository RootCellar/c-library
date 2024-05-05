#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

#define ANSI_COLOR_GRAY           "\x1b[90m"
#define ANSI_COLOR_BRIGHT_RED     "\x1b[91m"
#define ANSI_COLOR_BRIGHT_GREEN   "\x1b[92m"
#define ANSI_COLOR_BRIGHT_YELLOW  "\x1b[93m"
#define ANSI_COLOR_BRIGHT_BLUE    "\x1b[94m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_COLOR_BRIGHT_CYAN    "\x1b[96m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define FAIL_FAST 0

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

#endif
