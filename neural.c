#include "neural.h"

#include <time.h>

void main() {
  srand(time(NULL));

  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(2, 2);

  float onehalf = 0.5;
  float one = 1.0;

  float result = neural_evaluate(&neural_net, &one, 1);
  printf("Result: %f\n", result);

  struct Net_Training_Settings settings;
  settings.acceptable_error = 0.001;
  settings.learning_rate = 0.1;

  for(int i = 0; i < 500000; i++) {
    neural_train(&neural_net, settings, &one, 1, onehalf);
    neural_train(&neural_net, settings, &onehalf, 1, one);
  }

  result = neural_evaluate(&neural_net, &one, 1);
  printf("Result: %f\n", result);

  result = neural_evaluate(&neural_net, &onehalf, 1);
  printf("Result: %f\n", result);

  free_neural_net(&neural_net);
  tFreePointerList();
}
