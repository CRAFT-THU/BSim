/* This header file is writen by qp11
 * usually just for fun
 * Tue December 15 2015
 */
#ifndef GPU_KERNEL_H
#define GPU_KERNEL_H

#include "../neuron/GNeuron.h"
#include "../synapse/GSynapse.h"
#include "../net/GNetwork.h"

__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesFiredTable, int *c_gSynapsesLogTable);

__global__ void update_time();

__global__ void update_pre_synapse(N2SConnection *pConnection);

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id);

__global__ void find_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id);

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id);

__global__ void update_exp_hit(GExpSynapses *d_synapses, int num, int start_id);

__global__ void update_exp_synapse(GExpSynapses *d_synapses, int num, int start_id);

__global__ void update_basic_synapse(GBasicSynapses *d_synapses, int num, int start_id);
__global__ void update_alpha_synapse(GAlphaSynapses *d_synapses, int num, int start_id);

#endif /* GPU_KERNEL_H */

