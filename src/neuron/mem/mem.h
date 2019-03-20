
#ifndef MEM_H
#define MEM_H

#include "GMemNeurons.h"

__global__ void update_mem_neuron(GMemNeurons *d_neurons, int num, int start_id);

#endif // MEM_H
