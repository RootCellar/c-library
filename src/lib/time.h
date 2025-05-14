
#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <sys/time.h>

double timespec_to_double(struct timespec time);
double timespec_difference_seconds(struct timespec start, struct timespec end);
struct timespec get_time();

#endif //TIME_H
