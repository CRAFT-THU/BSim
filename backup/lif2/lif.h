
#ifndef LIF_H
#define LIF_H

#include "GLIFNeurons.h"

__global__ void find_lif_neuron(GLIFNeurons *data, real *currentE, real *currentI, int num, int offset);
__global__ void update_lif_neuron(Connection *conn, GLIFNeurons *data, real *currentI, real *currentE, int *fireTable, int *firedTableSizes, int num, int offset, int time);

// __global__ void update_all_life_neuron(GLIFNeurons *d_neurons, int num, int offset, int time);
// __global__ void update_dense_life_neuron(GLIFNeurons *d_neurons, int num, int offset, int time);

#endif // LIF_H

