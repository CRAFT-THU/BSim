
#ifndef MAX_H
#define MAX_H

#include "GMaxNeurons.h"

__global__ void update_max_neuron(GMaxNeurons *d_neurons, int num, int start_id, int time);

#endif // MAX_H

