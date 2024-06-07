
#ifndef NEURAL_H
#define NEURAL_H

#include <math.h>

#include "memory.h"

struct NeuralNet {

  int layers;
  int neurons_per_layer;

  struct Neuron** neurons;

};

struct Neuron {
  float* input_weights;
  int input_count;
};


#endif
