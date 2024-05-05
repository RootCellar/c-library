#ifndef STATISTICS_H
#define STATISTICS_H

#include "debug.h"

float statistics_average(float* values, size_t count) {

  float sum = 0;
  for(size_t i = 0; i < count; i++) {
    sum += values[i];
  }

  return sum / (float)count;
}

#endif
