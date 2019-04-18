
#ifndef ARRAY_H
#define ARRAY_H

#include "GArrayNeurons.h"

__global__ void update_array_neuron(GArrayNeurons *d_neurons, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time);

#endif // ARRAY_H
