/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
#include "../gpu_utils/gpu_func.h"
#include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/gpu_kernel.h"
#include "../net/MultiNetwork.h"
#include "MultiGPUSimulator.h"

pthread_barrier_t cycle_barrier;

CrossNodeData * global_cross_data;

MultiGPUSimulator::MultiGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

MultiGPUSimulator::~MultiGPUSimulator()
{
}

void *run_thread(void *para);
void *run_single(void *para);

int MultiGPUSimulator::run(real time)
{
	int sim_cycle = round(time/dt);
	reset();

	int device_count = 1;
	checkCudaErrors(cudaGetDeviceCount(&device_count));
	assert(device_count != 0);

	pthread_barrier_init(&cycle_barrier, NULL, device_count);

	MultiNetwork multiNet(network, device_count);
	DistriNetwork *node_nets = multiNet.buildNetworks();
	assert(node_nets != NULL);
	global_cross_data = multiNet.arrangeCrossNodeData(device_count);
	assert(global_cross_data != NULL);

	pthread_t *thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * device_count);
	assert(thread_ids != NULL);


	for (int i=0; i<device_count; i++) {
		node_nets[i]._sim_cycle = sim_cycle;
		node_nets[i]._node_idx = i;
		node_nets[i]._node_num = device_count;

		int ret = pthread_create(&(thread_ids[i]), NULL, &run_thread, (void*)&(node_nets[i]));
		assert(ret == 0);
	}

	for (int i=0; i<device_count; i++) {
		pthread_join(thread_ids[i], NULL);
	}

	pthread_barrier_destroy(&cycle_barrier);

	return 0;
}

void * run_thread(void *para) {
	DistriNetwork *network = (DistriNetwork*)para;

	char log_filename[512];
	sprintf(log_filename, "GSim_%d.log", network->_node_idx); 
	FILE *log_file = fopen(log_filename, "w+");
	assert(log_file != NULL);

	char v_filename[512];
	sprintf(v_filename, "g_v_%d.data", network->_node_idx); 
	FILE *v_file = fopen(v_filename, "w+");
	assert(v_file != NULL);

	checkCudaErrors(cudaSetDevice(network->_node_idx));

	GNetwork *pCpuNet = network->_network;
	GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	printf("Thread %d NeuronTypeNum: %d, SynapseTypeNum: %d\n", network->_node_idx, nTypeNum, sTypeNum);
	printf("Thread %d NeuronNum: %d, SynapseNum: %d\n", network->_node_idx, totalNeuronNum, totalSynapseNum);

	int dataOffset = network->_node_idx * network->_node_num;
	int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;

	int MAX_DELAY = pCpuNet->MAX_DELAY;
	//printf("MAX_DELAY: %d\n", pCpuNet->MAX_DELAY);

	GBuffers *buffers = alloc_buffers(pCpuNet->pN2SConnection->n_num, totalSynapseNum, MAX_DELAY);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
	BlockSize preSize = { 0, 0, 0};
	cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_pre_synapse, 0, totalNeuronNum); 
	preSize.gridSize = (upzero_else_set_one(totalNeuronNum) + (preSize.blockSize) - 1) / (preSize.blockSize);

	real *c_vm = hostMalloc<real>(totalNeuronNum);
	int life_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIFE);
	int lif_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIF);
	int copy_idx = -1;
	real *c_g_vm = NULL;

	if (life_idx >= 0) {
		GLIFENeurons *c_g_lif = copyFromGPU<GLIFENeurons>(static_cast<GLIFENeurons*>(c_pGpuNet->pNeurons[life_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
		copy_idx = life_idx;
	} else if (lif_idx >= 0) {
		GLIFNeurons *c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGpuNet->pNeurons[lif_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
		copy_idx = lif_idx;
	} else {
	}

	int * c_g_cross_id = gpuMalloc<int>(global_cross_data[dataIdx]._max_n_num); 


	vector<int> firedInfo;
	//printf("Thread %d: Start runing for %d cycles\n", network->_node_idx, network->_sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<network->_sim_cycle; time++) {
		//printf("Thread %d Cycle: %d\n", network->_node_idx, time);
		//fflush(stdout);

		for (int i=0; i<nTypeNum; i++) {
			//printf("Thread %d update neuron size %d %d\n", network->_node_idx, updateSize[c_pGpuNet->nTypes[i]].gridSize, updateSize[c_pGpuNet->nTypes[i]].blockSize);
			assert(c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i] > 0);
			cudaUpdateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
		}
		//printf("Thread %d before copy size %p\n", network->_node_idx, buffers->c_gFiredTableSizes);

		int currentIdx = time%(MAX_DELAY+1);
		//printf("Thread %d current idx %d\n", network->_node_idx, currentIdx);

		int copySize = 0;
		copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
		//printf("Thread %d copy size: %d\n", network->_node_idx, copySize);
		if (copySize > 0) {
			copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (pCpuNet->pN2SConnection->n_num*currentIdx), copySize);
		}

		if (copy_idx >= 0 && (c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]) > 0) {
			copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			//copyFromGPU<real>(c_I_syn, c_g_I_syn, c_pGpuNet->synapseNums[exp_idx+1]-c_pGpuNet->synapseNums[exp_idx]);
		}

		//printf("Thread %d before update connect\n", network->_node_idx);
		//printf("Thread %d update connect size %d %d\n", network->_node_idx, preSize.gridSize, preSize.blockSize);
		//printf("Thread %d after update connect\n", network->_node_idx);

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

		pthread_barrier_wait(&cycle_barrier);

		for (int i=0; i<network->_node_num; i++) {
			if (i != network->_node_idx) {
				memcpy(global_cross_data[dataIdx]._fired_n_idxs + global_cross_data[dataIdx]._fired_n_num, global_cross_data[dataOffset+i]._fired_n_idxs, global_cross_data[dataOffset+i]._fired_n_num * sizeof(int));
				global_cross_data[dataIdx]._fired_n_num += global_cross_data[dataOffset+i]._fired_n_num;
			}
		}

		
		if (global_cross_data[dataIdx]._fired_n_num > 0) {
			copyToGPU(c_g_cross_id, global_cross_data[dataIdx]._fired_n_idxs, global_cross_data[dataIdx]._fired_n_num);
			addCrossNeurons(c_g_cross_id, global_cross_data[dataIdx]._fired_n_num);
		}

		for (int i=0; i<copySize; i++) {
			fprintf(log_file, "%d ", buffers->c_neuronsFired[i]);
		}
		fprintf(log_file, "\n");

		for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
			fprintf(v_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(v_file, "\n");

		pthread_barrier_wait(&cycle_barrier);
		update_time<<<1, 1>>>();
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

	printf("Thread %d Simulation finesed in %ld:%ld:%ld.%06lds\n", network->_node_idx, hours, minutes, seconds, uSeconds);

	fclose(log_file);
	fclose(v_file);

	free_buffers(buffers);
	freeGPUNetwork(c_pGpuNet);

	return NULL;
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

	pthread_t *thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * device_count);
	assert(thread_ids != NULL);

	FILE **log_files = (FILE**)malloc(sizeof(FILE*)*device_count);
	FILE **v_files = (FILE**)malloc(sizeof(FILE*)*device_count);
	GBuffers **node_buffers = (GBuffers **)malloc(sizeof(GBuffers*)*device_count);
	BlockSize **node_update_sizes = (BlockSize **)malloc(sizeof(BlockSize*)*device_count);
	int **c_g_cross_ids = (int **)malloc(sizeof(int*)*device_count); 
	int *copy_idxs = (int *)malloc(sizeof(int)*device_count); 
	real **c_g_vms = (real**)malloc(sizeof(real*)*device_count);
	real **c_vms = (real**)malloc(sizeof(real*)*device_count);

	for (int i=0; i<device_count; i++) {
		node_nets[i]._sim_cycle = sim_cycle;
		node_nets[i]._node_idx = i;
		node_nets[i]._node_num = device_count;

		DistriNetwork *network = &(node_nets[i]);
		GNetwork *pCpuNet = network->_network;
		GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);
		int nTypeNum = pCpuNet->nTypeNum;
		int sTypeNum = pCpuNet->sTypeNum;
		int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
		int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
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

		node_buffers[i] = alloc_buffers(pCpuNet->pN2SConnection->n_num, totalSynapseNum, MAX_DELAY);
		node_update_sizes[i] = getBlockSize(totalNeuronNum, totalSynapseNum);
		c_g_cross_ids[i] = gpuMalloc<int>(global_cross_data[dataIdx]._max_n_num); 
		c_vms[i] = hostMalloc<real>(totalNeuronNum);
		copy_idxs[i] = getIndex(pCpuNet->nTypes, nTypeNum, LIFE);
		GLIFENeurons *c_g_lif = copyFromGPU<GLIFENeurons>(static_cast<GLIFENeurons*>(c_pGpuNet->pNeurons[copy_idxs[i]]), 1);
		c_g_vms[i] = c_g_lif->p_vm;

	}

	checkCudaErrors(cudaSetDevice(0));

	for (int time=0; time<sim_cycle; time++) {
		for (int node=0; node<device_count; node++) {
			DistriNetwork *network = &(node_nets[node]);
			GNetwork *pCpuNet = network->_network;
			GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);
			int nTypeNum = pCpuNet->nTypeNum;
			int sTypeNum = pCpuNet->sTypeNum;
			//int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
			//int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
			//int dataOffset = network->_node_idx * network->_node_num;
			int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;
			int MAX_DELAY = pCpuNet->MAX_DELAY;
			int *c_g_cross_id = c_g_cross_ids[node];

			GBuffers *buffers = node_buffers[node];
			BlockSize *updateSize = node_update_sizes[node];

			int copy_idx = copy_idxs[node];
			real *c_vm = c_vms[node];
			real *c_g_vm = c_g_vms[node];
			FILE *v_file = v_files[node];

			if (time > 0) {
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
				copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (pCpuNet->pN2SConnection->n_num*currentIdx), copySize);
			}

			if (copy_idx >= 0 && (c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]) > 0) {
				copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
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

			for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
				fprintf(v_file, "%.10lf \t", c_vm[i]);
			}
			fprintf(v_file, "\n");

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

	for (int i=0; i<device_count; i++) {
		fclose(log_files[i]);
		fclose(v_files[i]);
		free_buffers(node_buffers[i]);
	}



	return 0;
}
