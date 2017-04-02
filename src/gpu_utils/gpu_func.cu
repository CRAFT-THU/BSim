/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "../../include/GNeuron.h"
#include "../utils/utils.h"
#include "gpu_kernel.h"
#include "gpu_func.h"


int cudaUpdateConstant(void *data, int num, int start_id, BlockSize *pSize)
{
	update_constant_neuron<<<pSize->gridSize, pSize->blockSize>>>((GConstantNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdatePoisson(void *data, int num, int start_id, BlockSize *pSize)
{
	update_poisson_neuron<<<pSize->gridSize, pSize->blockSize>>>((GPoissonNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateArray(void *data, int num, int start_id, BlockSize *pSize)
{
	update_array_neuron<<<pSize->gridSize, pSize->blockSize>>>((GArrayNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateLIF(void *data, int num, int start_id, BlockSize *pSize)
{
	find_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, start_id);
	update_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateAllLIF(void *data, int num, int start_id, BlockSize *pSize)
{
	update_all_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateLIFE(void *data, int num, int start_id, BlockSize *pSize)
{
	find_life_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFENeurons*)data, num, start_id);
	update_life_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFENeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateAllLIFE(void *data, int num, int start_id, BlockSize *pSize)
{
	//update_all_life_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFENeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateMax(void *data, int num, int start_id, BlockSize *pSize)
{
	update_max_neuron<<<pSize->gridSize, pSize->blockSize>>>((GMaxNeurons*)data, num, start_id);

	return 0;
}

int cudaUpdateExp(void *data, int num, int start_id, BlockSize *pSize)
{
	update_exp_hit<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, start_id);
	reset_active_synapse<<<1, 1>>>();
	find_exp_synapse<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, start_id);
	update_exp_synapse<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, start_id);

	return 0;
}

int cudaUpdateAllExp(void *data, int num, int start_id, BlockSize *pSize)
{
	update_exp_hit<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, start_id);
	reset_active_synapse<<<1, 1>>>();
	update_all_exp_synapse<<<pSize->gridSize, pSize->blockSize>>>((GExpSynapses*)data, num, start_id);

	return 0;
}

int cudaUpdateStatic(void *data, int num, int start_id, BlockSize *pSize)
{
	update_static_hit<<<pSize->gridSize, pSize->blockSize>>>((GStaticSynapses*)data, num, start_id);
	reset_active_synapse<<<1, 1>>>();

	return 0;
}

int cudaUpdateAllStatic(void *data, int num, int start_id, BlockSize *pSize)
{
	update_static_hit<<<pSize->gridSize, pSize->blockSize>>>((GStaticSynapses*)data, num, start_id);
	reset_active_synapse<<<1, 1>>>();

	return 0;
}

int addCrossNeurons(int *ids, int num)
{
	add_cross_neuron<<<(num+MAXBLOCKSIZE-1)/MAXBLOCKSIZE, MAXBLOCKSIZE>>>(ids, num);
	return 0;
}

//int cudaUpdateAlpha(void *data, int num, int start_id, BlockSize *pSize)
//{
//	update_alpha_synapse<<<pSize->gridSize, pSize->blockSize>>>((GAlphaSynapses*)data, num, start_id);
//
//	return 0;
//}
//
//int cudaUpdateBasic(void *data, int num, int start_id, BlockSize *pSize)
//{
//	update_basic_synapse<<<pSize->gridSize, pSize->blockSize>>>((GBasicSynapses*)data, num, start_id);
//
//	return 0;
//}

BlockSize * getBlockSize(int nSize, int sSize)
{
	BlockSize *ret = (BlockSize*)malloc(sizeof(BlockSize)*TYPESIZE);
	cudaOccupancyMaxPotentialBlockSize(&(ret[Constant].minGridSize), &(ret[Constant].blockSize), update_constant_neuron, 0, nSize); 
	ret[Constant].gridSize = (upzero_else_set_one(nSize) + (ret[Constant].blockSize) - 1) / (ret[Constant].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Poisson].minGridSize), &(ret[Poisson].blockSize), update_poisson_neuron, 0, nSize); 
	ret[Poisson].gridSize = (upzero_else_set_one(nSize) + (ret[Poisson].blockSize) - 1) / (ret[Poisson].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Array].minGridSize), &(ret[Array].blockSize), update_poisson_neuron, 0, nSize); 
	ret[Array].gridSize = (upzero_else_set_one(nSize) + (ret[Array].blockSize) - 1) / (ret[Array].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[LIF].minGridSize), &(ret[LIF].blockSize), update_lif_neuron, 0, nSize); 
	ret[LIF].gridSize = (upzero_else_set_one(nSize) + (ret[LIF].blockSize) - 1) / (ret[LIF].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[LIFE].minGridSize), &(ret[LIFE].blockSize), update_life_neuron, 0, nSize); 
	ret[LIFE].gridSize = (upzero_else_set_one(nSize) + (ret[LIFE].blockSize) - 1) / (ret[LIFE].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Max].minGridSize), &(ret[Max].blockSize), update_max_neuron, 0, nSize); 
	ret[Max].gridSize = (upzero_else_set_one(nSize) + (ret[Max].blockSize) - 1) / (ret[Max].blockSize);

	//cudaOccupancyMaxPotentialBlockSize(&(ret[Basic].minGridSize), &(ret[Basic].blockSize), update_basic_synapse, 0, sSize); 
	//ret[Basic].gridSize = (sSize + (ret[Basic].blockSize) - 1) / (ret[Basic].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Exp].minGridSize), &(ret[Exp].blockSize), update_exp_synapse, 0, sSize); 
	ret[Exp].gridSize = (upzero_else_set_one(sSize) + (ret[Exp].blockSize) - 1) / (ret[Exp].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Static].minGridSize), &(ret[Static].blockSize), update_static_hit, 0, sSize); 
	ret[Static].gridSize = (upzero_else_set_one(sSize) + (ret[Static].blockSize) - 1) / (ret[Static].blockSize);

	//cudaOccupancyMaxPotentialBlockSize(&(ret[Alpha].minGridSize), &(ret[Alpha].blockSize), update_alpha_synapse, 0, sSize); 
	//ret[Alpha].gridSize = (sSize + (ret[Alpha].blockSize) - 1) / (ret[Alpha].blockSize);

	return ret;
}
