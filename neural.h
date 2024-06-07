
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

void free_neural_net(struct NeuralNet* neural_net) {
}

struct Neuron create_neuron(int input_count) {
}

struct NeuralNet create_neural_net(int layers, int neurons_per_layer) {
}

float activation_tanh(float output) {
  return tanh(output);
}

float neural_evaluate(struct NeuralNet* net, float* input) {

}

float neural_train(struct NeuralNet* net, float* input, float* correct_output) {

}

#endif