
#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <sys/time.h>

double timespec_to_double(struct timespec time) {
  double nanoseconds_in_seconds = (double) time.tv_nsec / 1000000000.0;
  double seconds = time.tv_sec;
  double timespec_in_seconds = seconds + nanoseconds_in_seconds;
  return timespec_in_seconds;
}

double timespec_difference_seconds(struct timespec start, struct timespec end) {
  // Shrink tv_sec down to the actual integer difference between them
  // Helps protect time measurements from floating point roundoff because
  // tv_sec is a large number
  end.tv_sec -= start.tv_sec;
  start.tv_sec = 0;

  return timespec_to_double(end) - timespec_to_double(start);
}

struct timespec get_time() {
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time;
}

#endif //TIME_H
