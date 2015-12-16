/* This header file is writen by qp11
 * usually just for fun
 * Tue December 15 2015
 */
#ifndef GPU_KERNEL_H
#define GPU_KERNEL_H

#include "Neuron.h"
#include "Synapse.h"
#include "Network.h"

__global__ void init_global(PlainNetwork * c_pGpuNet, unsigned int *c_gTimeTable, unsigned int *c_gFiredTable, bool *c_gSynapseFiredTable);

__global__ void update_postSynapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, unsigned int simTime);

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, unsigned int simTime);

__global__ void update_alpha_synapse(GLIFNeurons *d_neurons, GAlphaSynapses *d_synapses, unsigned int num, unsigned int simTime);

__global__ void update_exp_synapse(GLIFNeurons *d_neurons, GExpSynapses *d_synapses, unsigned int num, unsigned int simTime);

#endif /* GPU_KERNEL_H */

