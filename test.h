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

static int tests = 0;
static int tests_passed = 0;

#define TEST(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 0)
#define TEST_QUIET(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 1)

#define SECTION(name) \
        do { \
        printf("\n\n  " "%s ** " name " ** %s" "\n\n\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);\
        } while(0)

#define SHOW_TEST_RESULTS() show_test_results()

void pass_test(const char* expr, const char* file, const char* func, int line, const char* name, int quiet_pass) {
  tests_passed++;

  if(!quiet_pass) {
    printf("%sTest: %s -- %s -- PASS %s\n", TEST_PASS_OUTPUT_COLOR, name, expr, ANSI_COLOR_RESET);
  }
}

void fail_test(const char* expr, const char* file, const char* func, int line, const char* name) {
  printf("\n%sTest: %s -- %s -- FAILED %s\n", TEST_FAIL_OUTPUT_COLOR, name, expr, ANSI_COLOR_RESET);
  printf("%s ** TEST FAILED ** " "%s\n", TEST_FAIL_OUTPUT_COLOR, ANSI_COLOR_RESET);
  printf("%s ** %s line %d in %s() ** " "%s\n", TEST_FAIL_OUTPUT_COLOR, file, line, func, ANSI_COLOR_RESET);

  if(FAIL_FAST) {
    exit(EXIT_FAILURE);
  }
}

void handle_test_result(const char* expr, int result, const char* file, const char* func, int line, const char* name, int quiet_pass) {
  tests++;

  if(result) {
    pass_test(expr, file, func, line, name, quiet_pass);
  }
  else {
    fail_test(expr, file, func, line, name);
  }
}

void show_test_results() {
  printf("\n\n%s %d PASSED%s\n", TEST_PASS_OUTPUT_COLOR, tests_passed, ANSI_COLOR_RESET);

  int failed = tests - tests_passed;
  char* outputColor = TEST_FAIL_OUTPUT_COLOR;
  if(failed == 0) {
    outputColor = TEST_PASS_OUTPUT_COLOR;
  }

  printf("%s %d FAILED%s\n", outputColor, failed, ANSI_COLOR_RESET);
}

float float_abs(float value) {
  if(value < 0.0) return value * -1.0;
  return value;
}

int floats_equal(float one, float two) {
  return float_abs(one - two) < FLOAT_DIFFERENCE_TOLERANCE;
}

#endif
