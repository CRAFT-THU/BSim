/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */



int addCrossNeurons(int *ids, int num, int time)
{
	add_cross_neuron<<<(num+MAX_BLOCK_SIZE-1)/MAX_BLOCK_SIZE, MAX_BLOCK_SIZE>>>(ids, num, time);
	return 0;
}


int cudaDeliverNeurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num, int neuron_num, int time)
{
	deliver_neurons<<<(neuron_num + MAX_BLOCK_SIZE-1)/MAX_BLOCK_SIZE, MAX_BLOCK_SIZE>>>(idx2index, crossnode_index2idx, global_cross_data, fired_n_num, node_num, time);

	return 0;
}

BlockSize * getBlockSize(int nSize, int sSize)
{
	BlockSize *ret = (BlockSize*)malloc(sizeof(BlockSize)*TYPESIZE);
	memset(ret, 0, sizeof(BlockSize)*TYPESIZE);

	cudaOccupancyMaxPotentialBlockSize(&(ret[Array].minGridSize), &(ret[Array].blockSize), update_array_neuron, 0, nSize); 
	ret[Array].gridSize = (upzero_else_set_one(nSize) + (ret[Array].blockSize) - 1) / (ret[Array].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[LIF].minGridSize), &(ret[LIF].blockSize), update_lif_neuron, 0, nSize); 
	ret[LIF].gridSize = (upzero_else_set_one(nSize) + (ret[LIF].blockSize) - 1) / (ret[LIF].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Constant].minGridSize), &(ret[Constant].blockSize), update_constant_neuron, 0, nSize); 
	// ret[Constant].gridSize = (upzero_else_set_one(nSize) + (ret[Constant].blockSize) - 1) / (ret[Constant].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Poisson].minGridSize), &(ret[Poisson].blockSize), update_poisson_neuron, 0, nSize); 
	// ret[Poisson].gridSize = (upzero_else_set_one(nSize) + (ret[Poisson].blockSize) - 1) / (ret[Poisson].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Decide].minGridSize), &(ret[Decide].blockSize), update_max_neuron, 0, nSize); 
	// ret[Decide].gridSize = (upzero_else_set_one(nSize) + (ret[Decide].blockSize) - 1) / (ret[Decide].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[FFT].minGridSize), &(ret[FFT].blockSize), update_fft_neuron, 0, nSize); 
	// ret[FFT].gridSize = (upzero_else_set_one(nSize) + (ret[FFT].blockSize) - 1) / (ret[FFT].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Mem].minGridSize), &(ret[Mem].blockSize), update_mem_neuron, 0, nSize); 
	// ret[Mem].gridSize = (upzero_else_set_one(nSize) + (ret[Mem].blockSize) - 1) / (ret[Mem].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Max].minGridSize), &(ret[Max].blockSize), update_max_neuron, 0, nSize); 
	// ret[Max].gridSize = (upzero_else_set_one(nSize) + (ret[Max].blockSize) - 1) / (ret[Max].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[TJ].minGridSize), &(ret[TJ].blockSize), update_tj_neuron, 0, nSize); 
	// ret[TJ].gridSize = (upzero_else_set_one(nSize) + (ret[TJ].blockSize) - 1) / (ret[TJ].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Static].minGridSize), &(ret[Static].blockSize), update_static_hit, 0, sSize); 
	ret[Static].blockSize = 128;
	ret[Static].gridSize = (upzero_else_set_one(nSize) + (ret[Static].blockSize) - 1) / (ret[Static].blockSize);

	return ret;
}
