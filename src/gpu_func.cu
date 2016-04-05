/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "Neuron.h"
#include "gpu_kernel.h"
#include "gpu_func.h"

int updateLIFNeuron(void *, int, int);
int updateAlphaSynapses(void *, int, int);
int updateExpSynapses(void *, int, int);

void* (*createType[])() = { createLIFNeurons, /*createNengoNeurons, createInputNeurons, createPossionNeurons, createProbeNeurons,*/ createAlphaSynapses, createExpSynapses/*, createLowpassSynapses*/ };

size_t (*getSize[])() = { getLIFSize, /*getNengoSize, getInputSize, getPossionSize, getProbeSize,*/ getAlphaSize, getExpSize/*, getLowpassSize*/ };

int (*allocType[])(void *, int) = { allocLIFNeurons, /*allocNengoNeurons, allocInputNeurons, allocPossionNeurons, allocProbeNeurons,*/ allocAlphaSynapses, allocExpSynapses/*, allocLowpassSynapses*/ };

int (*allocConnect[])(void *, int *, int *, int *, int) = { allocLIFConnects, /*allocNengoConnects, allocInputConnects, allocPossionConnects, allocProbeConnects,*/ allocAlphaConnects, allocExpConnects/*, allocLowpassConnects*/ };

int (*cudaAllocType[])(void *, void *) = { cudaAllocLIFNeurons, /*cudaAllocNengoNeurons, cudaAllocInputNeurons, cudaAllocPossionNeurons, cudaAllocProbeNeurons,*/ cudaAllocAlphaSynapses, cudaAllocExpSynapses/*, cudaAllocLowpassSynapses*/ };

int (*cudaFreeType[])(void *) = { cudaFreeLIFNeurons, /*cudaFreeNengoNeurons, cudaFreeInputNeurons, cudaFreePossionNeurons, cudaFreeProbeNeurons,*/ cudaFreeAlphaSynapses, cudaFreeExpSynapses/*, cudaFreeLowpassSynapses*/ };

int (*updateType[])(void *, int, int) = { updateLIFNeuron, updateAlphaSynapses, updateExpSynapses };

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
