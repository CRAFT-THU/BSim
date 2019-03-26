
#include "../utils/constant.h"

struct GBuffers {
	// Neuron Arrays
	real *c_gNeuronInput;
	real *c_gNeuronInput_I;
	// Neuron Tables
	int *c_gFiredTable;
	int *c_gFiredTableSizes;
	int *c_gActiveTable;
	// Synapse Tables
	// int *c_gSynapsesActiveTable;
	// int *c_gSynapsesLogTable;

	int *c_neuronsFired;
	int *c_synapsesFired;

	int *c_gLayerInput;
	real *c_gXInput;

	int *c_gFireCount;
};

// void init_buffers(GBuffers * buf);
GBuffers* alloc_buffers(int neuron_num, int synapse_num, int max_delay, real dt);
int free_buffers(GBuffers *buf);
