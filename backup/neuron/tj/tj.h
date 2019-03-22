
#ifndef TJ_H
#define TJ_H

#include "GTJNeurons.h"

__global__ void update_tj_neuron(GTJNeurons *d_neurons, int num, int start_id, int time);

#endif // TJ_H

