
#ifndef CONSTANT_H
#define CONSTANT_H 

#include "GConstantNeurons.h"

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id);

#endif // CONSTANT_H

