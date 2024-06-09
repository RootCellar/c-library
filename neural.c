#include "neural.h"
#include "statistics.h"
#include "time.h"

#include <time.h>

void main() {
  srand(time(NULL));

  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(3, 3);

  float inputs[] = {1.0, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0};
  float outputs[] = {-1.0, 1.0, -1.0, -1.0};

  struct Net_Training_Settings settings;
  settings.acceptable_error = 0.01;
  settings.learning_rate = 1.0;

  unsigned long num_inputs = sizeof(inputs) / sizeof(float);
  unsigned long num_outputs = sizeof(outputs) / sizeof(float);

  if(num_inputs != num_outputs) {
    printf("mismatch between number of inputs and outputs!\n");
    return;
  }

  int training = 1;
  float result;
  float errors[num_inputs];
  struct timespec start, now;
  start = get_time();

  while(training) {
    for(int i = 0; i < num_inputs; i++) {
      neural_train(&neural_net, settings, &inputs[i], 1, outputs[i]);
      errors[i] = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
    }

    float max_error = statistics_max(errors, num_inputs);
    if(max_error < settings.acceptable_error) {
      // may be done?

      // check again
      for(int i = 0; i < num_inputs; i++) {
        errors[i] = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
      }

      max_error = statistics_max(errors, num_inputs);
      if(max_error < settings.acceptable_error) {
        training = 0;
      }

    }

    now = get_time();
    if(timespec_difference_seconds(start, now) >= 1.0) {
      start = get_time();

      for(int i = 0; i < num_inputs; i++) {
        result = neural_evaluate(&neural_net, &inputs[i], 1);
        errors[i] = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
        printf("%f: %f %f\n", inputs[i], result, errors[i]);
      }

      printf("Max error: %f\n", max_error);
    }
  }

  for(int i = 0; i < num_inputs; i++) {
    result = neural_evaluate(&neural_net, &inputs[i], 1);
    errors[i] = (float) fabs(neural_error(&neural_net, &inputs[i], 1, outputs[i]));
    printf("%f: %f %f\n", inputs[i], result, errors[i]);
  }

  free_neural_net(&neural_net);
  tFreePointerList();
}
