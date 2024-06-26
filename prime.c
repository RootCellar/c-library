#include <math.h>
#include <float.h>
#include <time.h>

#include "benchmark.h"
#include "test.h"
#include "statistics.h"

int isPrime(int num) {
  if(num <= 1) return 0;
  if(num == 2) return 1;

  int stop = (int) sqrt(num);
  for(int i = 2; i <= stop; i++) {
    if(num % i == 0) return 0;
  }
  return 1;
}

int isPrime_new(int num) {
  if(num <= 1) return 0;
  if(num == 2 || num == 3) return 1;
  
  int remainder = num % 6;
  if(remainder != 5 && remainder != 1) return 0;

  for(int i = 5; i * i <= num + 1; i+=6) {
    if(num % ( i + 2 ) == 0) return 0;
    if(num % i == 0) return 0;
  }

  return 1;
}

#define TEST_PRIME_FUNCTION(function) \
TEST( function(2) == 1, "2 is prime");\
TEST( function(3) == 1, "3 is prime");\
TEST( function(5) == 1, "5 is prime");\
TEST( function(37) == 1, "37 is prime");\
\
TEST( function(1) == 0, "1 is not prime");\
TEST( function(4) == 0, "4 is not prime");\
TEST( function(10) == 0, "10 is not prime");\
TEST( function(25) == 0, "25 is not prime");\
TEST( function(40) == 0, "40 is not prime");\
TEST( function(1000) == 0, "1000 is not prime");\


void main() {

  benchmark_setupLocale();
  srand(time(NULL));

  SECTION("Normal isPrime()");

  TEST_PRIME_FUNCTION(isPrime);

  SECTION("New isPrime_new()");

  TEST_PRIME_FUNCTION(isPrime_new);

  printf("\n\n");

  int test_reach = 50000;
  int benchmark_loops = 1000;
  int random;

  debug_printf("Float max: %f", FLT_MAX);
  debug_printf("CLOCKS_PER_SEC: %d", CLOCKS_PER_SEC);

  float* results = malloc( sizeof(float) * test_reach );
  float* results_new = malloc( sizeof(float) * test_reach );

  if(results == NULL || results_new == NULL) {
    printf("Could not allocate memory for results lists! \n");
    exit(1);
  }
  
  for(int i = 0; i < test_reach; i++) {
    random = rand() % 1000000;
    
    do {
      BENCHMARK_LOOPS_CODE(isPrime(random), results[i], benchmark_loops);
    } while(results[i] > FLT_MAX);
    
    if(results[i] > FLT_MAX) {
      debug_printf("%d came up with inf", random);
    }

    do {
      BENCHMARK_LOOPS_CODE(isPrime_new(random), results_new[i], benchmark_loops);
    } while(results_new[i] > FLT_MAX);
    
    if(results_new[i] > FLT_MAX) {
      debug_printf("%d came up with inf", random);
    }
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

  average = statistics_average(results_new, test_reach);
  max = statistics_max(results_new, test_reach);
  min = statistics_min(results_new, test_reach);
  stddev = statistics_standard_deviation(results_new, test_reach);
  avdev = statistics_average_deviation(results_new, test_reach);
  printf(" average: %'f \n", average);
  printf(" max: %'f \n", max);
  printf(" min: %'f \n", min);
  printf(" stddev: %'f \n", stddev);
  printf(" avdev: %'f \n", avdev);
  printf("\n\n");

  exit(0);
  
}
