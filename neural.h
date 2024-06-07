
#ifndef NEURAL_H
#define NEURAL_H

#include <math.h>

#include "memory.h"

struct Neuron {
  float* input_weights;
  int input_count;
};

struct NeuralNet {

  int layers;
  int neurons_per_layer;

  struct Neuron** neurons;
  struct Neuron output_neuron;

};

void free_neuron(struct Neuron* neuron) {
  if(neuron->input_weights != NULL) {
    tFree(neuron->input_weights);
    neuron->input_weights = NULL;
  }
}

void free_neural_net(struct NeuralNet* neural_net) {
  if(neural_net->neurons == NULL) return;

  debug_printf("Freeing neural net with %d layers and %d neurons per layer", neural_net->layers, neural_net->neurons_per_layer);

  for(int i = 0; i < neural_net->layers; i++) {
    for(int k = 0; k < neural_net->neurons_per_layer; k++) {
      struct Neuron neuron = neural_net->neurons[i][k];
      if(neuron.input_weights != NULL) {
        debug_printf("Freeing neuron %d in layer %d", k, i);
        free_neuron(&neuron);
      }
    }
  }

  for(int i = 0; i < neural_net->layers; i++) {
    if(neural_net->neurons[i] != NULL) {
      debug_printf("Freeing neuron layer %d", i);
      tFree(neural_net->neurons[i]);
      neural_net->neurons[i] = NULL;
    }
  }

  debug_print("Freeing neuron layer list");
  tFree(neural_net->neurons);
  neural_net->neurons = NULL;

  if(neural_net->output_neuron.input_weights != NULL) {
    debug_print("Freeing output neuron");
    free_neuron(&neural_net->output_neuron);
  }
  debug_print("Neural net freed and cleared");
}

struct Neuron create_neuron(int input_count) {
  struct Neuron neuron;
  neuron.input_count = input_count;

  unsigned long weights_list_bytes = sizeof(float) * input_count;

  neuron.input_weights = tMalloc(weights_list_bytes);
  if(neuron.input_weights == NULL) {
    debug_printf("Failed to allocate memory for a neuron with %d inputs!", input_count);
    return neuron;
  }
  memset(neuron.input_weights, 0, weights_list_bytes);

  debug_printf("Created a neuron with %d inputs", input_count);

  return neuron;
}

struct NeuralNet create_neural_net(int layers, int neurons_per_layer) {

  debug_printf("Creating neural net with %d layers and %d neurons per layer...", layers, neurons_per_layer);

  struct NeuralNet neural_net;
  neural_net.layers = layers;
  neural_net.neurons_per_layer = neurons_per_layer;

  unsigned long layers_list_bytes = sizeof(struct Neuron*) * layers;
  unsigned long neuron_list_bytes = sizeof(struct Neuron) * neurons_per_layer;

  neural_net.neurons = tMalloc(layers_list_bytes);
  if(neural_net.neurons == NULL) {
    debug_printf("Failed to create neural layers!", layers, neurons_per_layer);
    return neural_net;
  }
  memset(neural_net.neurons, 0, layers_list_bytes);

  debug_print("Created neural layer list");

  for(int i = 0; i < layers; i++) {
    neural_net.neurons[i] = tMalloc(neuron_list_bytes);
    if(neural_net.neurons[i] == NULL) {
      debug_printf("Failed to allocate layer %d!", i);
      free_neural_net(&neural_net);
      return neural_net;
    }
    memset(neural_net.neurons[i], 0, neuron_list_bytes);
  }

  debug_print("Allocated neural layers");

  for(int i = 0; i < layers; i++) {
    for(int k = 0; k < neurons_per_layer; k++) {
      neural_net.neurons[i][k] = create_neuron(neurons_per_layer);
      if(neural_net.neurons[i][k].input_weights == NULL) {
        debug_printf("Failed to allocate neuron %d in layer %d!", k, i);
        free_neural_net(&neural_net);
        return neural_net;
      }
    }
  }

  debug_print("Built neurons");

  neural_net.output_neuron = create_neuron(neurons_per_layer);
  if(neural_net.output_neuron.input_weights == NULL) {
    debug_print("Failed to allocate memory for output neuron");
    free_neural_net(&neural_net);
    return neural_net;
  }

  debug_print("Built output neuron");

  return neural_net;
}

float activation_tanh(float output) {
  return tanh(output);
}

float neural_evaluate(struct NeuralNet* net, float* input) {

}

float neural_train(struct NeuralNet* net, float* input, float* correct_output) {

}

#endif