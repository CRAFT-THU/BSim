/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "../neuron/GNeuron.h"
#include "gpu_kernel.h"
#include "gpu_func.h"

int updateConstantNeuron(void *data, int num, int simTime, BlockSize *pSize)
{
	update_constant_neuron<<<pSize->gridSize, pSize->blockSize>>>((GConstantNeurons*)data, num, simTime);

	return 0;
}

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

int updateBasicSynapses(void *data, int num, int simTime, BlockSize *pSize)
{
	update_basic_synapse<<<pSize->gridSize, pSize->blockSize>>>((GBasicSynapses*)data, num, simTime);

	return 0;
}

int (*updateType[])(void *, int, int, BlockSize*) = { updateConstantNeuron, updateLIFNeuron, updateBasicSynapses, updateAlphaSynapses, updateExpSynapses };

int (*cudaAllocType[])(void *, void *) = { cudaAllocConstantNeurons, cudaAllocLIFNeurons, /*cudaAllocNengoNeurons, cudaAllocInputNeurons, cudaAllocPossionNeurons, cudaAllocProbeNeurons,*/ cudaAllocBasicSynapses, cudaAllocAlphaSynapses, cudaAllocExpSynapses/*, cudaAllocLowpassSynapses*/ };

int (*cudaFreeType[])(void *) = { cudaFreeConstantNeurons, cudaFreeLIFNeurons, /*cudaFreeNengoNeurons, cudaFreeInputNeurons, cudaFreePossionNeurons, cudaFreeProbeNeurons,*/ cudaFreeBasicSynapses, cudaFreeAlphaSynapses, cudaFreeExpSynapses/*, cudaFreeLowpassSynapses*/ };

BlockSize * getBlockSize(int nSize, int sSize)
{
	BlockSize *ret = (BlockSize*)malloc(sizeof(BlockSize)*TypeSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Constant].minGridSize), &(ret[Constant].blockSize), update_lif_neuron, 0, nSize); 
	ret[Constant].gridSize = (nSize + (ret[Constant].blockSize) - 1) / (ret[Constant].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[LIF].minGridSize), &(ret[LIF].blockSize), update_lif_neuron, 0, nSize); 
	ret[LIF].gridSize = (nSize + (ret[LIF].blockSize) - 1) / (ret[LIF].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Basic].minGridSize), &(ret[Basic].blockSize), update_exp_synapse, 0, sSize); 
	ret[Basic].gridSize = (sSize + (ret[Basic].blockSize) - 1) / (ret[Basic].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Alpha].minGridSize), &(ret[Alpha].blockSize), update_alpha_synapse, 0, sSize); 
	ret[Alpha].gridSize = (sSize + (ret[Alpha].blockSize) - 1) / (ret[Alpha].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Exp].minGridSize), &(ret[Exp].blockSize), update_exp_synapse, 0, sSize); 
	ret[Exp].gridSize = (sSize + (ret[Exp].blockSize) - 1) / (ret[Exp].blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Alpha].minGridSize), &(ret[Alpha].blockSize), update_alpha_synapse, 0, sSize); 
	ret[Alpha].gridSize = (sSize + (ret[Alpha].blockSize) - 1) / (ret[Alpha].blockSize);

	return ret;
}
