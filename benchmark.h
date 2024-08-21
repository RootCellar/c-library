#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <locale.h>

#include "time.h"
#include "terminal_color.h"

#define SHOW_ALL 0

#define BENCHMARK_LOOPS(x, name, loops) do { struct timespec start_time = get_time();\
  for( int i = 0; i < loops; i++ ) { x; }\
  struct timespec end = get_time();\
  double seconds = timespec_difference_seconds(start_time, end); \
  printf(ANSI_COLOR_BRIGHT_CYAN name ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, seconds, ((float)loops/seconds)); } while(0)

#define BENCHMARK_LOOPS_CODE(x, var, loops) do { struct timespec __start_time = get_time();\
  for( int i = 0; i < loops; i++ ) { x; }\
  struct timespec __end = get_time();\
  double __seconds = timespec_difference_seconds(__start_time, __end); float __runs_per_second = ((float)loops/__seconds); \
  if(SHOW_ALL) printf(ANSI_COLOR_BRIGHT_CYAN #x ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, __seconds, __runs_per_second); var = __runs_per_second; } while(0)

// Do whatever x is the given number of times
#define TIMES(x, times) do { for(int i = 0; i < times; i++) x; } while(0)

void benchmark_setupLocale() {
  // set the locale so that printed numbers can be formatted
  // with commas separating the digits
  setlocale(LC_NUMERIC, "");
}

#endif
