#include <math.h>
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


int main() {

  benchmark_setupLocale();
  srand(time(NULL));

  SECTION("Normal isPrime()");

  TEST_PRIME_FUNCTION(isPrime);

  SECTION("New isPrime_new()");

  TEST_PRIME_FUNCTION(isPrime_new);

  SHOW_TEST_RESULTS();

  printf("\n\n");

  int test_reach = 50000;
  int benchmark_loops = 1000;
  int random;

  float* results = malloc( sizeof(float) * test_reach );
  float* results_new = malloc( sizeof(float) * test_reach );

  if(results == NULL || results_new == NULL) {
    printf("Could not allocate memory for results lists! \n");
    exit(1);
  }

  printf("Benchmarking isPrime() runs per second...\n");

  for(int i = 0; i < test_reach; i++) {
    random = rand() % 1000000;

    BENCHMARK_LOOPS_CODE(isPrime(random), results[i], benchmark_loops);
    BENCHMARK_LOOPS_CODE(isPrime_new(random), results_new[i], benchmark_loops);
  }

  printf("\n");
  printf("Original isPrime():\n");

  float average = statistics_average(results, test_reach);
  float max = statistics_max(results, test_reach);
  float min = statistics_min(results, test_reach);
  float stddev = statistics_standard_deviation(results, test_reach);
  float avdev = statistics_average_deviation(results, test_reach);
  printf(" average: %'20.0f \n", average);
  printf(" max:     %'20.0f \n", max);
  printf(" min:     %'20.0f \n", min);
  printf(" stddev:  %'20.0f \n", stddev);
  printf(" avdev:   %'20.0f \n", avdev);
  printf("\n\n");

  printf("New isPrime():\n");

  average = statistics_average(results_new, test_reach);
  max = statistics_max(results_new, test_reach);
  min = statistics_min(results_new, test_reach);
  stddev = statistics_standard_deviation(results_new, test_reach);
  avdev = statistics_average_deviation(results_new, test_reach);
  printf(" average: %'20.0f \n", average);
  printf(" max:     %'20.0f \n", max);
  printf(" min:     %'20.0f \n", min);
  printf(" stddev:  %'20.0f \n", stddev);
  printf(" avdev:   %'20.0f \n", avdev);
  printf("\n\n");

  return 0;
}
