
#ifndef DECIDE_H
#define DECIDE_H

#include "GDecideNeurons.h"


__global__ void update_decide_neuron(GDecideNeurons *d_neurons, int num, int start_id, int time);

#endif // DECIDE_H
