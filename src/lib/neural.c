
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "neural.h"
#include "memory.h"
#include "debug.h"
#include "sthread.h"

struct neural_train_thread_data {
  struct NeuralNet net;
  struct Net_Training_Settings settings;
  struct Net_Training_Item const * items;
  int item_count;

  float error;
};

long int get_rand() {
  return arc4random();
}

void apply_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, const size_t count) {
  for(size_t i = 0; i < count; i++) {
    struct NeuralNet_Weight_Adjustment* adjustment = &adjustments[i];
    if(!adjustment->applied) {
      adjustment->neuron->input_weights[adjustment->weight] += adjustment->amount;
      adjustment->applied = 1;
    }
  }
}

void undo_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, const size_t count) {
  for(size_t i = count; i > 0; i--) {
    struct NeuralNet_Weight_Adjustment* adjustment = &adjustments[i-1];
    if(adjustment->applied) {
      adjustment->neuron->input_weights[adjustment->weight] -= adjustment->amount;
      adjustment->applied = 0;
    }
  }
}

struct NeuralNet_Weight_Adjustment generate_weight_adjustment(struct Neuron const * neuron, const size_t weight, const float amount) {
  return (struct NeuralNet_Weight_Adjustment) { .neuron = neuron, .amount = amount, .weight = weight, .applied = 0};
}

void free_neuron(struct Neuron* neuron) {
  if(neuron->input_weights != NULL) {
    free(neuron->input_weights);
    neuron->input_weights = NULL;
  }
}

void free_neural_net(struct NeuralNet* neural_net) {
  if(neural_net->neurons == NULL) return;

  debug_printf("Freeing neural net with %zu layers and %zu neurons per layer", neural_net->layers,
               neural_net->neurons_per_layer);

  for(size_t i = 0; i < neural_net->layers; i++) {
    struct Neuron const * neuron_layer = neural_net->neurons[i];
    if(neuron_layer == NULL) continue;

    for(size_t k = 0; k < neural_net->neurons_per_layer; k++) {
      struct Neuron neuron = neuron_layer[k];
      if(neuron.input_weights != NULL) {
        free_neuron(&neuron);
      }
    }
  }

  debug_print("Freeing neuron layers");

  for(size_t i = 0; i < neural_net->layers; i++) {
    if(neural_net->neurons[i] != NULL) {
      free(neural_net->neurons[i]);
      neural_net->neurons[i] = NULL;
    }
  }

  debug_print("Freeing neuron layer list");
  free(neural_net->neurons);
  neural_net->neurons = NULL;

  if(neural_net->output_neuron.input_weights != NULL) {
    debug_print("Freeing output neuron");
    free_neuron(&neural_net->output_neuron);
  }

  debug_print("Neural net freed and cleared");
}

struct Neuron create_neuron(const size_t input_count) {
  struct Neuron neuron;
  neuron.input_count = input_count;

  const unsigned long weights_list_bytes = sizeof(float) * input_count;

  neuron.input_weights = malloc(weights_list_bytes);
  if(neuron.input_weights == NULL) {
    debug_printf("Failed to allocate memory for a neuron with %zu inputs!", input_count);
    return neuron;
  }
  memset(neuron.input_weights, 0, weights_list_bytes);

  for(size_t i = 0; i < input_count; i++) {
    neuron.input_weights[i] = 0.1f;
  }

  return neuron;
}

int copy_neuron(struct Neuron const * restrict from, struct Neuron const * restrict to) {
  if(from->input_weights == NULL || to->input_weights == NULL) return -1;
  if(from->input_count != to->input_count) return -1;

  for(size_t i = 0; i < from->input_count; i++) {
    to->input_weights[i] = from->input_weights[i];
  }

  return 0;
}

struct Neuron duplicate_neuron(const struct Neuron dup) {
  struct Neuron toRet = create_neuron(dup.input_count);
  if(toRet.input_weights == NULL) return toRet;

  if(copy_neuron(&dup, &toRet) < 0) free_neuron(&toRet);
  return toRet;
}

struct NeuralNet create_neural_net(const size_t layers, const size_t neurons_per_layer) {
  debug_printf("Creating neural net with %zu layers and %zu neurons per layer...", layers, neurons_per_layer);

  struct NeuralNet neural_net;
  neural_net.layers = layers;
  neural_net.neurons_per_layer = neurons_per_layer;

  // Initialize output neuron as blank
  neural_net.output_neuron.input_weights = NULL; neural_net.output_neuron.input_count = 0;

  const unsigned long layers_list_bytes = sizeof(struct Neuron*) * layers;
  const unsigned long neuron_list_bytes = sizeof(struct Neuron) * neurons_per_layer;

  neural_net.neurons = malloc(layers_list_bytes);
  if(neural_net.neurons == NULL) {
    debug_print("Failed to create neural layers!");
    return neural_net;
  }
  memset(neural_net.neurons, 0, layers_list_bytes);

  debug_print("Created neural layer list");

  for(size_t i = 0; i < layers; i++) {
    neural_net.neurons[i] = malloc(neuron_list_bytes);
    if(neural_net.neurons[i] == NULL) {
      debug_printf("Failed to allocate layer %zu!", i);
      free_neural_net(&neural_net);
      return neural_net;
    }
    memset(neural_net.neurons[i], 0, neuron_list_bytes);
  }

  debug_print("Allocated neural layers");

  for(size_t i = 0; i < layers; i++) {
    for(size_t k = 0; k < neurons_per_layer; k++) {
      neural_net.neurons[i][k] = create_neuron(neurons_per_layer);
      if(neural_net.neurons[i][k].input_weights == NULL) {
        debug_printf("Failed to allocate neuron %zu in layer %zu!", k, i);
        free_neural_net(&neural_net);
        return neural_net;
      }
    }
  }

  debug_printf("Built %zu neurons", layers * neurons_per_layer);

  neural_net.output_neuron = create_neuron(neurons_per_layer);
  if(neural_net.output_neuron.input_weights == NULL) {
    debug_print("Failed to allocate memory for output neuron");
    free_neural_net(&neural_net);
    return neural_net;
  }

  debug_print("Built output neuron");

  return neural_net;
}

int copy_neural_net(struct NeuralNet const * restrict from, struct NeuralNet const * restrict to) {
  debug_print("Attempting to copy neural net...");

  if(from->neurons == NULL || to->neurons == NULL) return -1;
  if(from->layers != to->layers) return -1;
  if(from->neurons_per_layer != to->neurons_per_layer) return -1;

  debug_print("Copying neurons...");

  for(size_t i = 0; i < from->layers; i++) {
    for(size_t k = 0; k < from->neurons_per_layer; k++) {
      if(copy_neuron(&from->neurons[i][k], &to->neurons[i][k]) == -1) return -1;
    }
  }

  debug_print("Copying output neuron...");

  if(copy_neuron(&from->output_neuron, &to->output_neuron) == -1) return -1;

  debug_print("Neural net copied.");

  return 0;
}

struct NeuralNet duplicate_neural_net(const struct NeuralNet dup) {
  debug_print("Attempting to duplicate neural net...");
  struct NeuralNet toRet = create_neural_net(dup.layers, dup.neurons_per_layer);
  if(toRet.neurons == NULL) return toRet;

  if(copy_neural_net(&dup, &toRet) == -1) {
    debug_print("Failed to duplicate Neural net!");
    free_neural_net(&toRet);
    return toRet;
  }
  debug_print("Neural net duplicated.");
  return toRet;
}

float activation_tanh(const float output) {
  return tanh(output);
}

float neuron_evaluate(const struct Neuron neuron, float const * input, const size_t input_count) {
  float output = 0.0;

  for(size_t i = 0; i < input_count && i < neuron.input_count; i++) {
    output += input[i] * neuron.input_weights[i];
  }

  return activation_tanh(output);
}

void neural_layer_evaluate(struct Neuron const * restrict layer, const size_t layer_count, float const * restrict input, const size_t input_count, float* restrict layer_outputs) {
  for(size_t i = 0; i < layer_count; i++) {
    layer_outputs[i] = neuron_evaluate(layer[i], input, input_count);
  }
}

float neural_evaluate(struct NeuralNet const * restrict net, float const * restrict input, const size_t input_count) {
  // Just return the output of the output neuron if there are no hidden layers
  if(net->layers < 1) {
    return neuron_evaluate(net->output_neuron, input, input_count);
  }

  float layer_outputs[net->neurons_per_layer];

  // Handle the first layer
  neural_layer_evaluate(net->neurons[0], net->neurons_per_layer, input, input_count, layer_outputs);

  // Handle every layer afterwards
  for(size_t i = 1; i < net->layers; i++) {
    float new_layer_outputs[net->neurons_per_layer];
    neural_layer_evaluate(net->neurons[i], net->neurons_per_layer, layer_outputs, net->neurons_per_layer,
                          new_layer_outputs);

    for(size_t k = 0; k < net->neurons_per_layer; k++) {
      layer_outputs[k] = new_layer_outputs[k];
    }
  }

  // Run the output neuron on the output of the last hidden layer
  return neuron_evaluate(net->output_neuron, layer_outputs, net->neurons_per_layer);
}

float neural_error(struct NeuralNet const * restrict net, float const * restrict input, const size_t input_count, const float correct_output) {
  const float result = neural_evaluate(net, input, input_count);
  const float error = correct_output - result;
  return error;
}

float neural_overall_error(struct NeuralNet const * restrict net, struct Net_Training_Item const * restrict items, const int count) {
  float overall_error = 0.0;

  for(int i = 0; i < count; i++) {
    const struct Net_Training_Item net_training_item = items[i];
    overall_error += (float) powf(neural_error(net, net_training_item.inputs, net_training_item.input_count,
                                               net_training_item.correct_output), 2.0f);
  }

  return overall_error;
}

float random_sign() {
  const long int x = get_rand() % 2;
  if(x == 0) return -1.0;
  return 1.0;
}

float sign_of(const float x) {
  if(x < 0.0) return -1.0;
  return 1.0;
}

float random_float() {
  float x = (float) (get_rand() % 20);
  x -= 10.0f;
  return x / 10.0f;
}

struct NeuralNet_Weight_Adjustment generate_random_weight_adjustment(struct NeuralNet const * net,
                                                                     const struct Net_Training_Settings settings) {
  struct Neuron const * neuron;

  const size_t weight = (size_t) get_rand() % net->neurons_per_layer;

  if(get_rand() % 10 < 1) {
    neuron = &(net->output_neuron);
  }
  else {
    const size_t layer = (size_t) get_rand() % net->layers;
    const size_t neuron_num = (size_t) get_rand() % net->neurons_per_layer;
    neuron = &(net->neurons[layer][neuron_num]);
  }

  const float amount = settings.learning_rate * random_float();

  return generate_weight_adjustment(neuron, weight, amount);
}

void neural_net_test_random_adjust(struct NeuralNet const * restrict net, struct Net_Training_Item const* restrict items, const int item_count,
                                   struct NeuralNet_Weight_Adjustment* restrict adjustments, const size_t adj_count) {
  const float error = neural_overall_error(net, items, item_count);

  apply_weight_adjustments(adjustments, adj_count);

  const float new_error = neural_overall_error(net, items, item_count);

  if(!(fabs(new_error) < fabs(error))) {
    undo_weight_adjustments(adjustments, adj_count);
  }
}

int neural_train(struct NeuralNet const * restrict net, const struct Net_Training_Settings settings, struct Net_Training_Item const * restrict items,
                 const int item_count) {
  const float error = neural_overall_error(net, items, item_count);

  // Output seems good
  if(fabs(error) < settings.acceptable_error) {
    return 0;
  }

  const size_t num_adjust = (size_t) get_rand() % settings.max_changes_at_once + 1;
  struct NeuralNet_Weight_Adjustment adjustments[num_adjust];
  for(size_t i = 0; i < num_adjust; i++) {
    adjustments[i] = generate_random_weight_adjustment(net, settings);
  }

  neural_net_test_random_adjust(net, items, item_count, adjustments, num_adjust);

  return 1;
}

static
void __neural_train_thread(const size_t id, const size_t thread_count, const size_t count, void* data) {
  struct neural_train_thread_data* thread_data_pointer = (struct neural_train_thread_data*) data;
  struct neural_train_thread_data* thread_data = &thread_data_pointer[id];

  for(size_t i = 0; i < thread_data->settings.iterations_per_thread; i++) {
    neural_train(&thread_data->net, thread_data->settings, thread_data->items, thread_data->item_count);
  }

  thread_data->error = neural_overall_error(&thread_data->net, thread_data->items, thread_data->item_count);
}

int neural_train_threaded(struct NeuralNet const * restrict net, const struct Net_Training_Settings settings, struct Net_Training_Item const * restrict items,
                          const int item_count) {
  const float original_error = neural_overall_error(net, items, item_count);

  // Output seems good
  if(fabs(original_error) < settings.acceptable_error) {
    return 0;
  }

  const size_t num_threads = settings.num_threads;
  if(num_threads < 1) return -1;

  struct neural_train_thread_data thread_args[num_threads];
  for(size_t i = 0; i < num_threads; i++) {
    thread_args[i].settings = settings;
    thread_args[i].items = items;
    thread_args[i].item_count = item_count;
    thread_args[i].net = duplicate_neural_net(*net);
  }

  run_in_threads(__neural_train_thread, thread_args, num_threads, num_threads);

  float thread_min_error = thread_args[0].error;
  float thread_error;
  size_t which_has_min = 0;
  for(size_t i = 1; i < num_threads; i++) {
    thread_error = thread_args[i].error;
    if(thread_error < thread_min_error) {
      thread_min_error = thread_error;
      which_has_min = i;
    }
  }

  if(thread_min_error < original_error) copy_neural_net(&thread_args[which_has_min].net, net);

  for(size_t i = 0; i < num_threads; i++) {
    free_neural_net(&thread_args[i].net);
  }

  return 1;
}
