
#include "../../utils/BlockSize.h"

#include "GMemNeurons.h"

__global__ void update_constant_neuron(GMemNeurons *d_neurons, int num, int start_id);

