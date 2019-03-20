
#ifndef STATIC_H
#define STATIC_H

#include "GStaticSynapses.h"

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id);
__global__ void update_static_hit(GStaticSynapses *d_synapses, int num, int start_id);

#endif // STATIC_H

