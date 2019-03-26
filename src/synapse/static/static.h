
#ifndef STATIC_H
#define STATIC_H

#include "../../net/Connection.h"
#include "GStaticSynapses.h"

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id, int time);
__global__ void update_static_hit(GStaticSynapses *d_synapses, int num, int start_id, int time);

#endif // STATIC_H

