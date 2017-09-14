/* This header file is writen by qp11
 * usually just for fun
 * Tue December 15 2015
 */
#ifndef GPU_KERNEL_H
#define GPU_KERNEL_H

#include "curand_kernel.h"
#include "curand.h"

#include "../../include/GNeuron.h"
#include "../../include/GSynapse.h"
#include "../net/GNetwork.h"


__global__ void init_connection(N2SConnection *pConnection);

__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesFiredTable, int *c_gSynapsesLogTable);

__global__ void update_time();
__global__ void init_time(int gCurrentCycle);

__global__ void curand_setup_kernel(curandState *state, int num);

__global__ void reset_active_synapse();

__global__ void add_cross_neuron(int *ids, int num);

__global__ void deliver_neurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num);


#endif /* GPU_KERNEL_H */

