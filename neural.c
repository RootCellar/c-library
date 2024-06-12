#include "neural.h"
#include "statistics.h"
#include "time.h"

#include <time.h>

void main() {
  srand(time(NULL));

  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(3, 3);
  struct NeuralNet copy_neural_net = duplicate_neural_net(neural_net);

  float inputs[] = {1.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.15, 0.25};
  float outputs[] = {-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, -1.0};

  struct Net_Training_Settings settings;
  settings.acceptable_error = 0.001;
  settings.learning_rate = 1.0;
  settings.max_changes_at_once = 2;

  unsigned long num_inputs = sizeof(inputs) / sizeof(float);
  unsigned long num_outputs = sizeof(outputs) / sizeof(float);

  if(num_inputs != num_outputs) {
    printf("mismatch between number of inputs and outputs!\n");
    return;
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

  int training = 1;
  unsigned long int rounds = 0;
  unsigned long int old_rounds = 0;
  float result;
  float error, overall_error = 0.0;
  struct timespec start, now;
  start = get_time();

  while(training) {

    settings.learning_rate = sinf((float) rounds / 1000) * 10.0;

    neural_train(&neural_net, settings, items, num_inputs);
    rounds++;

    overall_error = neural_overall_error(&neural_net, items, num_inputs);
    if(overall_error < settings.acceptable_error) {
      training = 0;
    }

    now = get_time();
    if(timespec_difference_seconds(start, now) >= 1.0) {
      start = get_time();

      settings.max_changes_at_once = rand() % 10 + 1;

      for(int i = 0; i < num_inputs; i++) {
        result = neural_evaluate(&neural_net, &inputs[i], 1);
        error = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
        printf("%f: %f %f\n", inputs[i], result, error);
      }

      printf("Overall error: %f\n", overall_error);
      printf("Rounds: %lu (%lu since last check)\n", rounds, rounds - old_rounds);
      old_rounds = rounds;
    }
  }

  printf("Finished training after %lu rounds.\n", rounds);

  for(int i = 0; i < num_inputs; i++) {
    result = neural_evaluate(&neural_net, &inputs[i], 1);
    error = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
    printf("%f: %f %f\n", inputs[i], result, error);
  }

  overall_error = neural_overall_error(&neural_net, items, num_inputs);
  printf("Overall error: %f\n", overall_error);

  free_neural_net(&neural_net);
  free_neural_net(&copy_neural_net);
  tFree(items);
  tFreePointerList();
}
