
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../lib/debug.h"
#include "../lib/benchmark.h"
#include "../lib/memory.h"
#include "../lib/neural.h"

#define BENCHMARK_TIMES (3)
#define BENCHMARK_ITERATIONS (1000 * 10 * 10)

int main() {
  srand(time(NULL));

  benchmark_setupLocale();

  struct NeuralNet neural_net = create_neural_net(3, 10);

  float inputs[] = {1.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95};
  float outputs[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};

  unsigned long num_inputs = sizeof(inputs) / sizeof(float);
  unsigned long num_outputs = sizeof(outputs) / sizeof(float);

  if(num_inputs != num_outputs) {
    printf("mismatch between number of inputs and outputs!\n");
    return 1;
  }

  struct Net_Training_Item* items;
  items = tMalloc(num_inputs * sizeof(struct Net_Training_Item));
  if(items == NULL) {
    printf("Could not allocate memory for neural training set!\n");
    free_neural_net(&neural_net);
    exit(1);
  }

  for(int i = 0; i < num_inputs; i++) {
    items[i].inputs = &inputs[i];
    items[i].input_count = 1;
    items[i].correct_output = outputs[i];
  }

  TIMES( BENCHMARK_LOOPS(neural_evaluate(&neural_net, inputs, num_inputs), "neural_evaluate", BENCHMARK_ITERATIONS), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS(neural_overall_error(&neural_net, items, num_inputs), "neural_overall_error", BENCHMARK_ITERATIONS), BENCHMARK_TIMES);

  tFreePointerList();
  exit(0);
}
