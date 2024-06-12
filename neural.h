
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

struct Net_Training_Settings {
  float learning_rate;
  float acceptable_error;
};

struct Net_Training_Item {
  float* inputs;
  int input_count;

  float correct_output;
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
        free_neuron(&neuron);
      }
    }

  }

  debug_print("Freeing neuron layers");

  for(int i = 0; i < neural_net->layers; i++) {
    if(neural_net->neurons[i] != NULL) {
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

  for(int i = 0; i < input_count; i++) {
    neuron.input_weights[i] = 0.1;
  }

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
    debug_print("Failed to create neural layers!");
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

  debug_printf("Built %d neurons", layers * neurons_per_layer);

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

  // Just return the output of the output neuron if there are no hidden layers
  if(net->layers < 1) {
    return neuron_evaluate(net->output_neuron, input, input_count);
  }

  float layer_outputs[net->neurons_per_layer];

  // Handle the first layer
  neural_layer_evaluate(net->neurons[0], net->neurons_per_layer, input, input_count, layer_outputs);

  // Handle every layer afterwards
  for(int i = 1; i < net->layers; i++) {
    float new_layer_outputs[net->neurons_per_layer];
    neural_layer_evaluate(net->neurons[i], net->neurons_per_layer, layer_outputs, net->neurons_per_layer, new_layer_outputs);

    for(int k = 0; k < net->neurons_per_layer; k++) {
      layer_outputs[k] = new_layer_outputs[k];
    }
  }

  // Run the output neuron on the output of the last hidden layer
  return neuron_evaluate(net->output_neuron, layer_outputs, net->neurons_per_layer);
}

float neural_error(struct NeuralNet* net, float* input, int input_count, float correct_output) {
  float result = neural_evaluate(net, input, input_count);
  float error = correct_output - result;
  return error;
}

float neural_overall_error(struct NeuralNet* net, struct Net_Training_Item* items, int count) {
  float overall_error = 0.0;

  for(int i = 0; i < count; i++) {
    struct Net_Training_Item net_training_item = items[i];
    overall_error += (float) powf(neural_error(net, net_training_item.inputs, net_training_item.input_count, net_training_item.correct_output),2.0f);
  }

  return overall_error;
}

float random_sign() {
  int x = rand() % 2;
  if(x == 0) return -1.0;
  return 1.0;
}

float sign_of(float x) {
  if(x < 0.0) return -1.0;
  return 1.0;
}

float random_float() {
  float x = rand() % 20;
  x -= 10.0;
  return x / 10.0;
}

void neuron_test_random_adjust(struct NeuralNet* net, struct Net_Training_Settings settings, struct Net_Training_Item* items, int count, struct Neuron* neuron) {
  for(int i = 0; i < neuron->input_count; i++) {
    if( fabs(neuron->input_weights[i] - 0.0) < 0.001 ) neuron->input_weights[i] = 0.01;
  }

  float error = neural_overall_error(net, items, count);

  int rand_weight = rand() % neuron->input_count;
  float orig_value = neuron->input_weights[rand_weight];

  neuron->input_weights[rand_weight] += settings.learning_rate * random_float();

  float new_error = neural_overall_error(net, items, count);

  if( fabs(new_error) > fabs(error) ) neuron->input_weights[rand_weight] = orig_value;
}

int neural_train(struct NeuralNet* net, struct Net_Training_Settings settings, struct Net_Training_Item* items, int count) {
  float error = neural_overall_error(net, items, count);

  // Output seems good
  if(fabs(error) < settings.acceptable_error) {
    return 0;
  }

  neuron_test_random_adjust(net, settings, items, count, &net->output_neuron);

  int rand_layer = rand() % net->layers;
  int rand_neuron = rand() % net->neurons_per_layer;
  struct Neuron* neuron = &(net->neurons[rand_layer][rand_neuron]);
  neuron_test_random_adjust(net, settings, items, count, neuron);

  return 1;
}

#endif