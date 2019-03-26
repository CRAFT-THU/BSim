
#ifndef MEM_H
#define MEM_H

#include "GMemNeurons.h"

__global__ void update_mem_neuron(GMemNeurons *d_neurons, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time);

#endif // MEM_H
