#include "neural.h"

#include <time.h>

void main() {
  srand(time(NULL));

  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(3, 3);

  float onehalf = 0.5;
  float onefifth = 1.0 / 5.0;
  float onethird = 1.0 / 3.0;
  float one = 1.0;
  float negone = -1.0;

  float result = neural_evaluate(&neural_net, &one, 1);
  printf("Result: %f\n", result);

  struct Net_Training_Settings settings;
  settings.acceptable_error = 0.001;
  settings.learning_rate = 0.1;

  for(int i = 0; i < 500000; i++) {
    neural_train(&neural_net, settings, &one, 1, negone);
    neural_train(&neural_net, settings, &onehalf, 1, one);
    neural_train(&neural_net, settings, &onethird, 1, negone);
    neural_train(&neural_net, settings, &onefifth, 1, negone);
  }

  result = neural_evaluate(&neural_net, &one, 1);
  printf("Result: %f\n", result);

  result = neural_evaluate(&neural_net, &onehalf, 1);
  printf("Result: %f\n", result);

  result = neural_evaluate(&neural_net, &onethird, 1);
  printf("Result: %f\n", result);

  result = neural_evaluate(&neural_net, &onefifth, 1);
  printf("Result: %f\n", result);

  free_neural_net(&neural_net);
  tFreePointerList();
}
