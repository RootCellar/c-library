#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <locale.h>
#include <time.h>

#include "terminal_color.h"

#define SHOW_ALL 0

#define BENCHMARK_TIME(x, name, time) do { int runCount = 0; clock_t start_time = clock(); clock_t time_now;\
    while( ( time_now = clock() ) - start_time < CLOCKS_PER_SEC * time) { x; ++runCount; }\
    float seconds = (float) (time_now - start_time) / CLOCKS_PER_SEC; \
    printf(ANSI_COLOR_BRIGHT_CYAN name ": %'d over %'f seconds " ANSI_COLOR_RESET "\n", runCount, seconds, ((float)runCount/seconds)); } while(0)

#define BENCHMARK_LOOPS(x, name, loops) do { clock_t start_time = clock();\
    for( int i = 0; i < loops; i++ ) { x; }\
    float seconds = (float) (clock() - start_time) / CLOCKS_PER_SEC; \
    printf(ANSI_COLOR_BRIGHT_CYAN name ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, seconds, ((float)loops/seconds)); } while(0)

#define BENCHMARK_LOOPS_CODE(x, var, loops) do { clock_t start_time = clock();\
    for( int i = 0; i < loops; i++ ) { x; }\
    float seconds = (float) (clock() - start_time) / CLOCKS_PER_SEC; float runs_per_second = ((float)loops/seconds); \
    if(SHOW_ALL) printf(ANSI_COLOR_BRIGHT_CYAN #x ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, seconds, runs_per_second); var = runs_per_second; } while(0)

// Do whatever x is the given number of times
#define TIMES(x, times) do { for(int i = 0; i < times; i++) x; } while(0)

void benchmark_setupLocale() {
    // set the locale so that printed numbers can be formatted
    // with commas separating the digits
    setlocale(LC_NUMERIC, "");
}

#endif
