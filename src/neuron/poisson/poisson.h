
#ifndef POISSON_H
#define POISSON_H

#include "GPoissonNeurons.h"

__global__ void update_poisson_neuron(GPoissonNeurons *d_neurons, int num, int start_id);

#endif // POISSON_H
