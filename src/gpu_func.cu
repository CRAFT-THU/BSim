/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "Neuron.h"
#include "gpu_kernel.h"
#include "gpu_func.h"


void* (*createType[])() = { createLIFNeurons, /*createNengoNeurons, createInputNeurons, createPossionNeurons, createProbeNeurons,*/ createAlphaSynapses, createExpSynapses/*, createLowpassSynapses*/ };

size_t (*getSize[])() = { getLIFSize, /*getNengoSize, getInputSize, getPossionSize, getProbeSize,*/ getAlphaSize, getExpSize/*, getLowpassSize*/ };

int (*allocType[])(void *, int) = { allocLIFNeurons, /*allocNengoNeurons, allocInputNeurons, allocPossionNeurons, allocProbeNeurons,*/ allocAlphaSynapses, allocExpSynapses/*, allocLowpassSynapses*/ };

int (*allocConnect[])(void *, int *, int *, int *, int) = { allocLIFConnects, /*allocNengoConnects, allocInputConnects, allocPossionConnects, allocProbeConnects,*/ allocAlphaConnects, allocExpConnects/*, allocLowpassConnects*/ };

int (*cudaAllocType[])(void *, void *) = { cudaAllocLIFNeurons, /*cudaAllocNengoNeurons, cudaAllocInputNeurons, cudaAllocPossionNeurons, cudaAllocProbeNeurons,*/ cudaAllocAlphaSynapses, cudaAllocExpSynapses/*, cudaAllocLowpassSynapses*/ };

int (*cudaFreeType[])(void *) = { cudaFreeLIFNeurons, /*cudaFreeNengoNeurons, cudaFreeInputNeurons, cudaFreePossionNeurons, cudaFreeProbeNeurons,*/ cudaFreeAlphaSynapses, cudaFreeExpSynapses/*, cudaFreeLowpassSynapses*/ };

int updateLIFNeuron(void *data, int num, int simTime, BlockSize *pSize)
{
	update_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, simTime);

	return 0;
}

int updateAlphaSynapses(void *data, int num, int simTime, BlockSize *pSize)
{
	update_alpha_synapse<<<pSize->gridSize, pSize->blockSize>>>((GAlphaSynapses*)data, num, simTime);

	return 0;
}

int updateExpSynapses(void *data, int num, int simTime, BlockSize *pSize)
{
	update_exp_synapse<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, simTime);

	return 0;
}

int (*updateType[])(void *, int, int, BlockSize*) = { updateLIFNeuron, updateAlphaSynapses, updateExpSynapses };

BlockSize * getBlockSize(int nSize, int sSize)
{
	BlockSize *ret = (BlockSize*)malloc(sizeof(BlockSize)*TypeSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[LIF].minGridSize), &(ret[LIF].blockSize), update_lif_neuron, 0, nSize); 
	ret[LIF].gridSize = (nSize + (ret[LIF].blockSize) - 1) / (ret[LIF].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Exp].minGridSize), &(ret[Exp].blockSize), update_exp_synapse, 0, sSize); 
	ret[Exp].gridSize = (sSize + (ret[Exp].blockSize) - 1) / (ret[Exp].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Alpha].minGridSize), &(ret[Alpha].blockSize), update_alpha_synapse, 0, sSize); 
	ret[Alpha].gridSize = (sSize + (ret[Alpha].blockSize) - 1) / (ret[Alpha].blockSize);

	return ret;
}
