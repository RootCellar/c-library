#include "neural.h"

void main() {
  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(8, 8);

  int count = 1;
  float input[count];

  input[0] = 1;
  float result = neural_evaluate(&neural_net, input, count);
  printf("Result: %f\n", result);

  free_neural_net(&neural_net);
  tFreePointerList();
}