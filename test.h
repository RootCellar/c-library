#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

#include "terminal_color.h"

#define FAIL_FAST 1

#define TEST_PASS_OUTPUT_COLOR ANSI_COLOR_BRIGHT_GREEN
#define TEST_FAIL_OUTPUT_COLOR ANSI_COLOR_BRIGHT_RED

#define FLOAT_DIFFERENCE_TOLERANCE (0.0001)

#define SHOW_LOCATION() \
        do { fprintf(stdout, "%s: %s() line %d \n\n", __FILE__, \
                                 __func__, __LINE__); } while(0)

#define TEST(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 0)
#define TEST_QUIET(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 1)

#define SECTION(name) \
        do { \
        printf("\n\n  " "%s ** " name " ** %s" "\n\n\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);\
        } while(0)

#define SHOW_TEST_RESULTS() show_test_results()

void pass_test(const char* expr, const char* file, const char* func, int line, const char* name, int quiet_pass);
void fail_test(const char* expr, const char* file, const char* func, int line, const char* name);
void handle_test_result(const char* expr, int result, const char* file, const char* func, int line, const char* name, int quiet_pass);

void show_test_results();

float float_abs(float value);
int floats_equal(float one, float two);

#endif
