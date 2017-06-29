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

struct GBuffers {
	// Neuron Arrays
	real *c_gNeuronInput;
	real *c_gNeuronInput_I;
	// Neuron Tables
	int *c_gFiredTable;
	int *c_gFiredTableSizes;
	int *c_gActiveTable;
	// Synapse Tables
	int *c_gSynapsesActiveTable;
	int *c_gSynapsesLogTable;

	int *c_neuronsFired;
	int *c_synapsesFired;

	int *c_gLayerInput;
	real *c_gXInput;

	int *c_gFireCount;
};

__global__ void init_connection(N2SConnection *pConnection);

__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesFiredTable, int *c_gSynapsesLogTable);

__global__ void update_time();
__global__ void init_time(int gCurrentCycle);

__global__ void curand_setup_kernel(curandState *state, int num);

__global__ void reset_active_synapse();

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id);

__global__ void update_poisson_neuron(GPoissonNeurons *d_neurons, int num, int start_id);

__global__ void update_array_neuron(GArrayNeurons *d_neurons, int num, int start_id);

__global__ void update_max_neuron(GMaxNeurons *d_neurons, int num, int start_id);

__global__ void find_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);

__global__ void update_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);
__global__ void update_all_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);
__global__ void update_dense_life_neuron(GLIFENeurons *d_neurons, int num, int start_id);

__global__ void update_tj_neuron(GTJNeurons *d_neurons, int num, int start_id);

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id);
__global__ void update_static_hit(GStaticSynapses *d_synapses, int num, int start_id);

__global__ void add_cross_neuron(int *ids, int num);

__global__ void deliver_neurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num);

void init_buffers(GBuffers * buf);
GBuffers* alloc_buffers(int neuron_num, int synapse_num, int max_delay);
int free_buffers(GBuffers *buf);

#endif /* GPU_KERNEL_H */

