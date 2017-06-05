
#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
#include "../gpu_utils/gpu_func.h"
#include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/gpu_kernel.h"
#include "../net/MultiNetwork.h"
#include "MultiGPUSimulator.h"

CrossNodeData * global_cross_data;

void deliverNeurons(DistriNetwork *network, GBuffers *buffers, int copySize)
{
	for (int i=0; i<network->_node_num; i++) {
		int offset = i * network->_node_num + network->_node_idx; 
		global_cross_data[offset]._fired_n_num = 0;
	}

	for (int i=0; i<copySize; i++) {
		int nid = buffers->c_neuronsFired[i];
		int tmp = network->_crossnode_map->_idx2index[nid];
		if (tmp >= 0) {
			for (int j=0; j<network->_node_num; j++) {
				int tmp2 = tmp * network->_node_num + j;
				int map_nid = network->_crossnode_map->_crossnode_index2idx[tmp2];
				if (map_nid >= 0) {
					//_node_idx to j 
					int offset = j * network->_node_num + network->_node_idx; 
					global_cross_data[offset]._fired_n_idxs[global_cross_data[offset]._fired_n_num] = map_nid; 
					global_cross_data[offset]._fired_n_num++;
				}
			}
		}

	}
}

void collectNeurons(DistriNetwork *network)
{
	int dataOffset = network->_node_idx * network->_node_num;
	int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;
	for (int i=0; i<network->_node_num; i++) {
		if ((i != network->_node_idx) && (global_cross_data[dataOffset+i]._fired_n_num > 0)) {
			memcpy(global_cross_data[dataIdx]._fired_n_idxs + global_cross_data[dataIdx]._fired_n_num, global_cross_data[dataOffset+i]._fired_n_idxs, global_cross_data[dataOffset+i]._fired_n_num * sizeof(int));
			global_cross_data[dataIdx]._fired_n_num += global_cross_data[dataOffset+i]._fired_n_num;
		}
	}
}

int MultiGPUSimulator::single_run(real time)
{
	int sim_cycle = round(time/dt);
	reset();

	int device_count = 1;
	checkCudaErrors(cudaGetDeviceCount(&device_count));
	assert(device_count != 0);

	MultiNetwork multiNet(network, device_count);
	DistriNetwork *node_nets = multiNet.buildNetworks();
	assert(node_nets != NULL);
	global_cross_data = multiNet.arrangeCrossNodeData(device_count);
	assert(global_cross_data != NULL);

	FILE **log_files = (FILE**)malloc(sizeof(FILE*)*device_count);
	FILE **v_files = (FILE**)malloc(sizeof(FILE*)*device_count);
	GBuffers **node_buffers = (GBuffers **)malloc(sizeof(GBuffers*)*device_count);
	BlockSize **node_update_sizes = (BlockSize **)malloc(sizeof(BlockSize*)*device_count);
	int **c_g_cross_ids = (int **)malloc(sizeof(int*)*device_count); 
	int *copy_idxs = (int *)malloc(sizeof(int)*device_count); 

	for (int i=0; i<device_count; i++) {
		node_nets[i]._sim_cycle = sim_cycle;
		node_nets[i]._node_idx = i;
		node_nets[i]._node_num = device_count;

		DistriNetwork *network = &(node_nets[i]);
		GNetwork *pCpuNet = network->_network;
		int nTypeNum = pCpuNet->nTypeNum;
		int sTypeNum = pCpuNet->sTypeNum;
		//int nodeNeuronNum = pCpuNet->neuronNums[nTypeNum];
		int allNeuronNum = pCpuNet->pN2SConnection->n_num;
		int nodeSynapseNum = pCpuNet->synapseNums[sTypeNum];
		int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;
		int MAX_DELAY = pCpuNet->MAX_DELAY;

		char log_filename[512];
		sprintf(log_filename, "GSim_single_%d.log", node_nets[i]._node_idx); 
		FILE *log_file = fopen(log_filename, "w+");
		assert(log_file != NULL);
		log_files[i] = log_file;

		char v_filename[512];
		sprintf(v_filename, "g_v_single_%d.data", node_nets[i]._node_idx); 
		FILE *v_file = fopen(v_filename, "w+");
		assert(v_file != NULL);
		v_files[i] = v_file;

		node_buffers[i] = alloc_buffers(allNeuronNum, nodeSynapseNum, MAX_DELAY);
		node_update_sizes[i] = getBlockSize(allNeuronNum, nodeSynapseNum);
		c_g_cross_ids[i] = gpuMalloc<int>(global_cross_data[dataIdx]._max_n_num); 
		copy_idxs[i] = getIndex(pCpuNet->nTypes, nTypeNum, LIFE);
	}

	checkCudaErrors(cudaSetDevice(0));

	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<sim_cycle; time++) {
		for (int node=0; node<device_count; node++) {
			DistriNetwork *network = &(node_nets[node]);
			GNetwork *pCpuNet = network->_network;
			GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);
			int nTypeNum = pCpuNet->nTypeNum;
			int sTypeNum = pCpuNet->sTypeNum;
			int allNeuronNum = pCpuNet->pN2SConnection->n_num;
			int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;
			int MAX_DELAY = pCpuNet->MAX_DELAY;
			int *c_g_cross_id = c_g_cross_ids[node];

			
			GBuffers *buffers = node_buffers[node];
			init_buffers(buffers);
			init_connection<<<1, 1>>>(c_pGpuNet->pN2SConnection);
			BlockSize *updateSize = node_update_sizes[node];

			int copy_idx = copy_idxs[node];
			FILE *v_file = v_files[node];
			FILE *log_file = log_files[node];

			if (time > 0) {
				init_time<<<1, 1>>>(time - 1);
				if (global_cross_data[dataIdx]._fired_n_num > 0) {
					copyToGPU(c_g_cross_id, global_cross_data[dataIdx]._fired_n_idxs, global_cross_data[dataIdx]._fired_n_num);
					addCrossNeurons(c_g_cross_id, global_cross_data[dataIdx]._fired_n_num);
				}
				update_time<<<1, 1>>>();

			}
			
			for (int i=0; i<nTypeNum; i++) {
				assert(c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i] > 0);
				cudaUpdateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
			}

			int currentIdx = time%(MAX_DELAY+1);

			int copySize = 0;
			copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
			if (copySize > 0) {
				copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (allNeuronNum*currentIdx), copySize);
			}

			for (int i=0; i<sTypeNum; i++) {
				assert(c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i] > 0);
				cudaUpdateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], &updateSize[pCpuNet->nTypes[i]]);
			}

			for (int i=0; i<network->_node_num; i++) {
				int offset = i * network->_node_num + network->_node_idx; 
				global_cross_data[offset]._fired_n_num = 0;
			}

			for (int i=0; i<copySize; i++) {
				int nid = buffers->c_neuronsFired[i];
				int tmp = network->_crossnode_map->_idx2index[nid];
				if (tmp >= 0) {
					for (int j=0; j<network->_node_num; j++) {
						int tmp2 = tmp * network->_node_num + j;
						int map_nid = network->_crossnode_map->_crossnode_index2idx[tmp2];
						if (map_nid >= 0) {
							//_node_idx to j 
							int offset = j * network->_node_num + network->_node_idx; 
							global_cross_data[offset]._fired_n_idxs[global_cross_data[offset]._fired_n_num] = map_nid; 
							global_cross_data[offset]._fired_n_num++;
						}
					}
				}

			}

			if (copy_idx >= 0 && (c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]) > 0) {
				GLIFENeurons *c_g_lif = copyFromGPU<GLIFENeurons>(static_cast<GLIFENeurons*>(c_pGpuNet->pNeurons[copy_idx]), 1);
				real *c_vm = copyFromGPU<real>(c_g_lif->p_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
				for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
					fprintf(v_file, "%.10lf \t", c_vm[i]);
				}
				fprintf(v_file, "\n");
				free(c_g_lif);
				free(c_vm);
			}

			for (int i=0; i<copySize; i++) {
				fprintf(log_file, "%d ", buffers->c_neuronsFired[i]);
			}
			fprintf(log_file, "\n");

			fetchNetworkFromGPU(pCpuNet, c_pGpuNet);

			freeGPUNetwork(c_pGpuNet);
		}

		for (int node=0; node<device_count; node++) {
			DistriNetwork *network = &(node_nets[node]);
			int dataOffset = network->_node_idx * network->_node_num;
			int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;
			for (int i=0; i<network->_node_num; i++) {
				if (i != network->_node_idx) {
					memcpy(global_cross_data[dataIdx]._fired_n_idxs + global_cross_data[dataIdx]._fired_n_num, global_cross_data[dataOffset+i]._fired_n_idxs, global_cross_data[dataOffset+i]._fired_n_num * sizeof(int));
					global_cross_data[dataIdx]._fired_n_num += global_cross_data[dataOffset+i]._fired_n_num;
				}
			}
		}

	}
	gettimeofday(&te, NULL);
	long seconds = te.tv_sec - ts.tv_sec;
	long hours = seconds/3600;
	seconds = seconds%3600;
	long minutes = seconds/60;
	seconds = seconds%60;
	long uSeconds = te.tv_usec - ts.tv_usec;
	if (uSeconds < 0) {
		uSeconds += 1000000;
		seconds = seconds - 1;
	}

	printf("Simulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);


	for (int i=0; i<device_count; i++) {
		DistriNetwork *network = &(node_nets[i]);
		GNetwork *pCpuNet = network->_network;
		int nTypeNum = pCpuNet->nTypeNum;
		int nodeNeuronNum = pCpuNet->neuronNums[nTypeNum];
		GBuffers *buffers = node_buffers[i];

		int *rate = (int*)malloc(sizeof(int)*nodeNeuronNum);
		copyFromGPU<int>(rate, buffers->c_gFireCount, nodeNeuronNum);

		char fire_filename[512];
		sprintf(fire_filename, "GFire_single_%d.log", i); 
		FILE *rate_file = fopen(fire_filename, "w+");
		if (rate_file == NULL) {
			printf("Open file Sim.log failed\n");
			return -1;
		}

		for (int i=0; i<nodeNeuronNum; i++) {
			fprintf(rate_file, "%d \t", rate[i]);
		}

		free(rate);
		fclose(rate_file);
	}

	for (int i=0; i<device_count; i++) {
		fclose(log_files[i]);
		fclose(v_files[i]);
		free_buffers(node_buffers[i]);
	}



	return 0;
}
