#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "benchmark.h"

#define BENCHMARK_TIMES (2)

int hit(float baseCrit, float* modifier) {
  float critChance = baseCrit + (*modifier);
  
  int roll = rand() % 1000;
  int rollValue = (int) (critChance * 1000);
  
  if(roll <= rollValue) {
    (*modifier) = 0.0;
    return 1;
  }
  (*modifier) += 0.03;
  return 0;
}

int main() {
  srand(time(NULL));

  benchmark_setupLocale();

  // Hit

  float modifier = 0.0;
  TIMES( BENCHMARK_LOOPS( hit(0.05,&modifier), "hit", 1000000 ), BENCHMARK_TIMES);

  // rand

  TIMES( BENCHMARK_LOOPS( rand(), "rand", 1000000 ), BENCHMARK_TIMES);

  // clock

  TIMES( BENCHMARK_LOOPS( clock(), "clock", 1000000 ), BENCHMARK_TIMES);
  
}
