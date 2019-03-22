
#ifndef CONSTANTS_H
#define CONSTANTS_H 

#include "GConstantNeurons.h"

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id, int time);

#endif // CONSTANTS_H

