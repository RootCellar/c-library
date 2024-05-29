#ifndef STATISTICS_H
#define STATISTICS_H

#include <math.h>

#include "debug.h"

float statistics_float_abs(float value) {
  if(value < 0.0) return value * -1.0;
  return value;
}

float statistics_max(float* values, size_t count) {
  if(count < 1) return 0.0;

  float max = values[0];
  for(size_t i = 1; i < count; i++) {
    if(values[i] > max) max = values[i];
  }

  return max;
}

float statistics_min(float* values, size_t count) {
  if(count < 1) return 0.0;

  float min = values[0];
  for(size_t i = 1; i < count; i++) {
    if(values[i] < min) min = values[i];
  }

  return min;
}

float statistics_average(float* values, size_t count) {
  if(count < 1) return 0.0;

  float sum = 0;
  for(size_t i = 0; i < count; i++) {
    sum += values[i];
  }

  return sum / (float)count;
}

float statistics_average_deviation(float* values, size_t count) {
  if(count < 1) return 0.0;

  float average = statistics_average(values, count);

  float deviation_sum = 0;
  for(size_t i = 0; i < count; i++) {
    deviation_sum += statistics_float_abs( values[i] - average );
  }

  return deviation_sum / (float)count;
}

float statistics_standard_deviation(float* values, size_t count) {
  if(count < 1) return 0.0;
  
  float average = statistics_average(values, count);

  float deviation_sum = 0;
  for(size_t i = 0; i < count; i++) {
    deviation_sum += pow(statistics_float_abs( values[i] - average ), 2.0);
  }

  return sqrtf(deviation_sum / (float)count);
}

#endif
