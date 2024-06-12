
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
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
  int max_changes_at_once;
};

struct Net_Training_Item {
  float* inputs;
  int input_count;

  float correct_output;
};

struct NeuralNet_Weight_Adjustment {
  float amount;
  int weight;

  struct Neuron* neuron;

  int applied;
};

void apply_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, int count) {
  for(int i = 0; i < count; i++) {
    struct NeuralNet_Weight_Adjustment* adjustment = &adjustments[i];
    if(!adjustment->applied) {
      adjustment->neuron->input_weights[adjustment->weight] += adjustment->amount;
      adjustment->applied = 1;
    }
  }
}

void undo_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, int count) {
  for(int i = count - 1; i >= 0; i--) {
    struct NeuralNet_Weight_Adjustment* adjustment = &adjustments[i];
    if(adjustment->applied) {
      adjustment->neuron->input_weights[adjustment->weight] -= adjustment->amount;
      adjustment->applied = 0;
    }
  }
}

struct NeuralNet_Weight_Adjustment generate_weight_adjustment(struct Neuron* neuron, int weight, int amount) {
  struct NeuralNet_Weight_Adjustment adjustment;
  adjustment.neuron = neuron;
  adjustment.amount = amount;
  adjustment.weight = weight;
  adjustment.applied = 0;

  return adjustment;
}

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

int copy_neuron(struct Neuron* from, struct Neuron* to) {
  if(from->input_weights == NULL || to->input_weights == NULL) return -1;
  if(from->input_count != to->input_count) return -1;

  for(int i = 0; i < from->input_count; i++) {
    to->input_weights[i] = from->input_weights[i];
  }

  return 0;
}

struct Neuron duplicate_neuron(struct Neuron dup) {
  struct Neuron toRet = create_neuron(dup.input_count);
  if(toRet.input_weights == NULL) return toRet;

  if(copy_neuron(&dup, &toRet) < 0) free_neuron(&toRet);
  return toRet;
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

int copy_neural_net(struct NeuralNet* from, struct NeuralNet* to) {
  debug_print("Attempting to copy neural net...");

  if(from->neurons == NULL || to->neurons == NULL) return -1;
  if(from->layers != to->layers) return -1;
  if(from->neurons_per_layer != to->neurons_per_layer) return -1;

  debug_print("Copying neurons...");

  for(int i = 0; i < from->layers; i++) {
    for(int k = 0; k < from->neurons_per_layer; k++) {
      if(copy_neuron(&from->neurons[i][k], &to->neurons[i][k]) == -1) return -1;
    }
  }

  debug_print("Copying output neuron...");

  if(copy_neuron(&from->output_neuron, &to->output_neuron) == -1) return -1;

  debug_print("Neural net copied.");

  return 0;
}

struct NeuralNet duplicate_neural_net(struct NeuralNet dup) {
  debug_print("Attempting to duplicate neural net...");
  struct NeuralNet toRet = create_neural_net(dup.layers, dup.neurons_per_layer);
  if(toRet.neurons == NULL) return toRet;

  copy_neural_net(&dup, &toRet);
  debug_print("Neural net duplicated.");
  return toRet;
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

void neural_net_test_random_adjust(struct NeuralNet* net, struct Net_Training_Item* items, int item_count, struct NeuralNet_Weight_Adjustment* adjustments, int adj_count) {
  float error = neural_overall_error(net, items, item_count);

  apply_weight_adjustments(adjustments, adj_count);

  float new_error = neural_overall_error(net, items, item_count);

  if( fabs(new_error) > fabs(error) ) {
    undo_weight_adjustments(adjustments, adj_count);
  }
}

struct NeuralNet_Weight_Adjustment generate_random_weight_adjustment(struct NeuralNet* net, struct Net_Training_Settings settings) {
  struct Neuron* neuron;

  int weight = rand() % net->neurons_per_layer;

  if(rand() % 10 < 2) {
    neuron = &(net->output_neuron);
  }
  else {
    int layer = rand() % net->layers;
    int neuron_num = rand() % net->neurons_per_layer;
    neuron = &(net->neurons[layer][neuron_num]);
  }

  float amount = settings.learning_rate * random_float();

  return generate_weight_adjustment(neuron, weight, amount);
}

int neural_train(struct NeuralNet* net, struct Net_Training_Settings settings, struct Net_Training_Item* items, int item_count) {
  float error = neural_overall_error(net, items, item_count);

  // Output seems good
  if(fabs(error) < settings.acceptable_error) {
    return 0;
  }

  int num_adjust = rand() % settings.max_changes_at_once + 1;
  // int num_adjust = 1;
  struct NeuralNet_Weight_Adjustment adjustments[num_adjust];
  for(int i = 0; i < num_adjust; i++) {
    adjustments[i] = generate_random_weight_adjustment(net, settings);
  }

  neural_net_test_random_adjust(net, items, item_count, adjustments, num_adjust);

  return 1;
}

#endif