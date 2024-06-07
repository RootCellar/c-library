#include "neural.h"

void main() {
  tResize(8192);

  struct NeuralNet neural_net = create_neural_net(8, 8);

  free_neural_net(&neural_net);
  tFreePointerList();
}