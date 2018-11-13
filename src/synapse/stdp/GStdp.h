
#include "GStdpSynapses.h"

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id);
__global__ void update_static_hit(GStaticSynapses *d_synapses, int num, int start_id);

