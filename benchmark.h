#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "terminal_color.h"

#define SHOW_ALL 0

#define BENCHMARK_LOOPS(x, name, loops) do { struct timespec start_time = get_time();\
    for( int i = 0; i < loops; i++ ) { x; }\
    struct timespec end = get_time();\
    end.tv_sec -= start_time.tv_sec; start_time.tv_sec=0; \
    double seconds = timespec_to_double(end) - timespec_to_double(start_time); \
    printf(ANSI_COLOR_BRIGHT_CYAN name ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, seconds, ((float)loops/seconds)); } while(0)

#define BENCHMARK_LOOPS_CODE(x, var, loops) do { struct timespec start_time = get_time();\
    for( int i = 0; i < loops; i++ ) { x; }\
    struct timespec end = get_time();\
    end.tv_sec -= start_time.tv_sec; start_time.tv_sec=0; \
    double seconds = timespec_to_double(end) - timespec_to_double(start_time); float __runs_per_second = ((float)loops/seconds); \
    if(SHOW_ALL) printf(ANSI_COLOR_BRIGHT_CYAN #x ": %'d over %'f seconds ( %'f per second ) " ANSI_COLOR_RESET "\n", loops, seconds, __runs_per_second); var = __runs_per_second; } while(0)

// Do whatever x is the given number of times
#define TIMES(x, times) do { for(int i = 0; i < times; i++) x; } while(0)

void benchmark_setupLocale() {
    // set the locale so that printed numbers can be formatted
    // with commas separating the digits
    setlocale(LC_NUMERIC, "");
}

double timespec_to_double(struct timespec time) {
    double x2 = (double) time.tv_nsec / 1000000000.0;
    double x1 = time.tv_sec;
    double x = x1 + x2;
    // printf("%ld %ld\n", time.tv_sec, time.tv_nsec);
    // printf("%lf %lf %lf\n", x, x1, x2);
    return x;
}

struct timespec get_time() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time;
}

#endif
