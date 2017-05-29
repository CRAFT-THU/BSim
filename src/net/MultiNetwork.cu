
#include "../third_party/cuda/helper_cuda.h"
#include "MultiNetwork.h"

//Alloc cross device gmem, will set device to 0 before return
CrossNodeDataGPU* MultiNetwork::arrangeCrossNodeDataGPU(int node_num)
{
	CrossNodeDataGPU * cross_data = static_cast<CrossNodeDataGPU*>(malloc(sizeof(CrossNodeDataGPU)));
	assert(cross_data != NULL);

	cross_data->_max_num = static_cast<int*>(malloc(sizeof(int)*node_num*node_num));
	assert(cross_data->_max_num != NULL);
	cross_data->_fired_arrays = static_cast<int**>(malloc(sizeof(int*)*node_num*node_num));
	assert(cross_data->_fired_arrays != NULL);

	checkCudaErrors(cudaMallocHost((void**)&(cross_data->_fired_num), sizeof(int)*node_num*node_num));
	checkCudaErrors(cudaMemset(cross_data->_fired_num, 0, sizeof(int)*node_num*node_num));


	for (int i=0; i<_node_num; i++) {
		for (int j=0; j<_node_num; j++) {
			// i->j 
			int i2j = i * _node_num + j;
			cross_data._fired_num = 0;

			int count = 0;
			for (auto iter = _crossnode_neurons_send[i].begin(); iter != _crossnode_neurons_send[i].end(); iter++) {
				if (_crossnode_neurons_recv[j].find(*iter) != _crossnode_neurons_recv[j].end()) {
					count++;
				}
			}
			cross_data->_max_num[i2j] = count;
			if (count > 0) {
				checkCudaErrors(cudaMalloc((void**)&(cross_data->_fired_arrays[i2j]), sizeof(int)*count));
			} else {
				cross_data->_fired_arrays[i2j] = NULL;
			}
		}
	}

	return cross_data;
}

