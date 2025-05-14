#ifndef STATISTICS_H
#define STATISTICS_H

float statistics_float_abs(float value);
float statistics_max(float* values, size_t count);
float statistics_min(float* values, size_t count);
float statistics_average(float* values, size_t count);
float statistics_average_deviation(float* values, size_t count);
float statistics_standard_deviation(float* values, size_t count);

#endif
