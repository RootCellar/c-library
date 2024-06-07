
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
    struct Neuron* neuron_layer = neural_net->neurons[i];
    if(neuron_layer == NULL) continue;

    for(int k = 0; k < neural_net->neurons_per_layer; k++) {
      struct Neuron neuron = neuron_layer[k];
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

float neuron_evaluate(struct Neuron neuron, float* input, int input_count) {
  float output = 0.0;

  for(int i = 0; i < input_count && i < neuron.input_count; i++) {
    output += input[i] * neuron.input_weights[i];
  }

  return activation_tanh(output);
}

void neural_layer_evaluate(struct Neuron* layer, int layer_count, float* input, int input_count, float* layer_outputs) {
  for(int i = 0; i < layer_count; i++) {
    layer_outputs[i] = neuron_evaluate(layer[i], input, input_count);
  }
}

float neural_evaluate(struct NeuralNet* net, float* input, int input_count) {

  if(net->layers < 1) {
    return neuron_evaluate(net->output_neuron, input, input_count);
  }

  float layer_outputs[net->neurons_per_layer];

  // Handle the first layer
  neural_layer_evaluate(net->neurons[0], net->neurons_per_layer, input, input_count, layer_outputs);

  if(net->layers == 1) {
    return neuron_evaluate(net->output_neuron, layer_outputs, net->neurons_per_layer);
  }

  for(int i = 1; i < net->layers; i++) {
    float new_layer_outputs[net->neurons_per_layer];
    neural_layer_evaluate(net->neurons[i], net->neurons_per_layer, layer_outputs, net->neurons_per_layer, new_layer_outputs);

    for(int i = 0; i < net->neurons_per_layer; i++) {
      layer_outputs[i] = new_layer_outputs[i];
    }
  }

  return neuron_evaluate(net->output_neuron, layer_outputs, net->neurons_per_layer);
}

float neural_train(struct NeuralNet* net, float* input, int input_count, float correct_output) {

}

#endif