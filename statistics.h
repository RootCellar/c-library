#ifndef STATISTICS_H
#define STATISTICS_H

#include <math.h>

#include "debug.h"

float statistics_float_abs(float value) {
  if(value < 0.0) return value * -1.0;
  else return value;
}

float statistics_average(float* values, size_t count) {

  float sum = 0;
  for(size_t i = 0; i < count; i++) {
    sum += values[i];
  }

  return sum / (float)count;
}

float statistics_average_deviation(float* values, size_t count) {

  float average = statistics_average(values, count);

  float deviation_sum = 0;
  for(size_t i = 0; i < count; i++) {
    deviation_sum += statistics_float_abs( values[i] - average );
  }

  return deviation_sum / (float)count;
}

float statistics_standard_deviation(float* values, size_t count) {
  float average = statistics_average(values, count);

  float deviation_sum = 0;
  for(size_t i = 0; i < count; i++) {
    deviation_sum += pow(statistics_float_abs( values[i] - average ), 2.0);
  }

  return (float) sqrt((double) deviation_sum / (double)count);
}

#endif
