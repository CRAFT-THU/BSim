/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "Neuron.h"
#include "gpu_kernel.h"
#include "GFunc.h"

int updateLIFNeuron(void *data, int num, int simTime)
{
	update_lif_neuron<<<3, 2, 0>>>((GLIFNeurons*)data, num, simTime);

	return 0;
}

int updateAlphaSynapses(void *data, int num, int simTime)
{
	update_alpha_synapse<<<1, 1, 0>>>((GAlphaSynapses*)data, num, simTime);

	return 0;
}

int updateExpSynapses(void *data, int num, int simTime)
{
	update_exp_synapse<<<1, 1, 0>>>((GExpSynapses*)data, num, simTime);

	return 0;
}
