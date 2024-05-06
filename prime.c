#include <math.h>

#include "benchmark.h"
#include "test.h"
#include "statistics.h"

int isPrime(int num) {
  if(num == 1 || num == 2) return 1;

  int stop = (int) sqrt(num);
  for(int i = 2; i <= stop; i++) {
    if(num % i == 0) return 0;
  }
  return 1;
}

int isPrime_new(int num) {
  if(num == 1 || num == 2 || num == 3) return 1;
  
  int remainder = num % 6;
  if(remainder != 5 && remainder != 1) return 0;

  int stop = (int) sqrt(num);
  for(int i = 6; i <= stop + 1; i+=6) {
    if(num % ( i + 1 ) == 0) return 0;
    if(num % ( i - 1 ) == 0) return 0;
  }

  return 1;
}

#define TEST_PRIME_FUNCTION(function) \
TEST( function(1) == 1, "1 is prime");\
TEST( function(2) == 1, "2 is prime");\
TEST( function(3) == 1, "3 is prime");\
TEST( function(5) == 1, "5 is prime");\
TEST( function(37) == 1, "37 is prime");\
\
TEST( function(4) == 0, "4 is not prime");\
TEST( function(10) == 0, "10 is not prime");\
TEST( function(25) == 0, "25 is not prime");\
TEST( function(40) == 0, "40 is not prime");\
TEST( function(1000) == 0, "1000 is not prime");\


void main() {

  benchmark_setupLocale();

  SECTION("Normal isPrime()");

  TEST_PRIME_FUNCTION(isPrime);

  SECTION("New isPrime_new()");

  TEST_PRIME_FUNCTION(isPrime_new);

  printf("\n\n");

  size_t test_reach = 20000;

  float* results = malloc( sizeof(float) * test_reach );
  for(int i = 0; i < test_reach; i++) {
    BENCHMARK_LOOPS_CODE(isPrime(i+1), results[i], 1000);
  }
  float average = statistics_average(results, test_reach);
  float max = statistics_max(results, test_reach);
  float min = statistics_min(results, test_reach);
  float stddev = statistics_standard_deviation(results, test_reach);
  float avdev = statistics_average_deviation(results, test_reach);
  printf(" average: %'f \n", average);
  printf(" max: %'f \n", max);
  printf(" min: %'f \n", min);
  printf(" stddev: %'f \n", stddev);
  printf(" avdev: %'f \n", avdev);
  printf("\n\n");

  for(int i = 0; i < test_reach; i++) {
    BENCHMARK_LOOPS_CODE(isPrime_new(i+1), results[i], 1000);
  }
  average = statistics_average(results, test_reach);
  max = statistics_max(results, test_reach);
  min = statistics_min(results, test_reach);
  stddev = statistics_standard_deviation(results, test_reach);
  avdev = statistics_average_deviation(results, test_reach);
  printf(" average: %'f \n", average);
  printf(" max: %'f \n", max);
  printf(" min: %'f \n", min);
  printf(" stddev: %'f \n", stddev);
  printf(" avdev: %'f \n", avdev);
  printf("\n\n");
}
