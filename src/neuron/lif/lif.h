
#ifndef LIF_H
#define LIF_H

#include "GLIFENeurons.h"

__global__ void findLIF(GLIFENeurons *d_neurons, int num, int offset);
__global__ void updateLIF(GLIFENeurons *d_neurons, real *currentI, real *currentE, int num, int offset, int time);

// __global__ void update_all_life_neuron(GLIFENeurons *d_neurons, int num, int offset, int time);
// __global__ void update_dense_life_neuron(GLIFENeurons *d_neurons, int num, int offset, int time);

#endif // LIF_H

