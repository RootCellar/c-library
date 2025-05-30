
#ifndef NEURAL_H
#define NEURAL_H

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
  int num_threads;
  int iterations_per_thread;
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

int get_rand();
float random_sign();
float random_float();
float sign_of(float x);

void apply_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, int count);
void undo_weight_adjustments(struct NeuralNet_Weight_Adjustment* adjustments, int count);

struct NeuralNet_Weight_Adjustment generate_weight_adjustment(struct Neuron* neuron, int weight, float amount);
struct NeuralNet_Weight_Adjustment generate_random_weight_adjustment(struct NeuralNet* net, struct Net_Training_Settings settings);

void free_neuron(struct Neuron* neuron);
void free_neural_net(struct NeuralNet* neural_net);

struct Neuron create_neuron(int input_count);
int copy_neuron(struct Neuron* from, struct Neuron* to);
struct Neuron duplicate_neuron(struct Neuron dup);

struct NeuralNet create_neural_net(int layers, int neurons_per_layer);
int copy_neural_net(struct NeuralNet* from, struct NeuralNet* to);
struct NeuralNet duplicate_neural_net(struct NeuralNet dup);

float activation_tanh(float output);

float neuron_evaluate(struct Neuron neuron, float* input, int input_count);
void neural_layer_evaluate(struct Neuron* layer, int layer_count, float* input, int input_count, float* layer_outputs);
float neural_evaluate(struct NeuralNet* net, float* input, int input_count);
float neural_error(struct NeuralNet* net, float* input, int input_count, float correct_output);
float neural_overall_error(struct NeuralNet* net, struct Net_Training_Item* items, int count);

void neural_net_test_random_adjust(struct NeuralNet* net, struct Net_Training_Item* items, int item_count,
                                   struct NeuralNet_Weight_Adjustment* adjustments, int adj_count);

int neural_train(struct NeuralNet* net, struct Net_Training_Settings settings, struct Net_Training_Item* items,
                 int item_count);

int neural_train_threaded(struct NeuralNet* net, struct Net_Training_Settings settings, struct Net_Training_Item* items,
                          int item_count);

#endif
