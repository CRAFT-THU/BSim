
#ifndef LIF_H
#define LIF_H

#include "GLIFENeurons.h"

__global__ void find_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);
__global__ void update_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);
__global__ void update_all_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);
__global__ void update_dense_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);

#endif // LIF_H

