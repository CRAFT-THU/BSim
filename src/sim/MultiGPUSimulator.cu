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

	char logFilename[512];
	sprintf(logFilename, "GSim_%d.log", network->_node_idx); 
	FILE *logFile = fopen(logFilename, "w+");
	assert(logFile != NULL);

	char dataFilename[512];
	sprintf(dataFilename, "GSim_%d.data", network->_node_idx); 
	FILE *dataFile = fopen(dataFilename, "w+");
	assert(dataFile != NULL);

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

	GBuffers *buffers = alloc_buffers(totalNeuronNum, totalSynapseNum, MAX_DELAY);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
	BlockSize preSize = { 0, 0, 0};
	BlockSize postSize = { 0, 0, 0};
	cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_lif_neuron, 0, totalNeuronNum); 
	preSize.gridSize = (totalNeuronNum + (preSize.blockSize) - 1) / (preSize.blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(postSize.minGridSize), &(postSize.blockSize), update_lif_neuron, 0, totalSynapseNum); 
	postSize.gridSize = (totalSynapseNum + (postSize.blockSize) - 1) / (postSize.blockSize);

	real *c_vm = hostMalloc<real>(totalNeuronNum);
	int lif_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIF);
	GLIFNeurons *c_g_lif;
	real *c_g_vm;
	if (lif_idx > 0) {
		c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGpuNet->pNeurons[lif_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
	}
	int * c_g_cross_id = gpuMalloc<int>(global_cross_data[dataIdx]._max_n_num);
	//real *c_I_syn = hostMalloc<real>(totalSynapseNum);
	//int exp_idx = getIndex(pCpuNet->sTypes, sTypeNum, Exp);
	//GExpSynapses *c_g_exp = copyFromGPU<GExpSynapses>(static_cast<GExpSynapses*>(c_pGpuNet->pSynapses[exp_idx]), 1);
	//real *c_g_I_syn = c_g_exp->p_I_syn;


	vector<int> firedInfo;
	//printf("Start runing for %d cycles\n", network->simCycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<network->_sim_cycle; time++) {
		//printf("\rCycle: %d", time);
		//fflush(stdout);

		for (int i=0; i<nTypeNum; i++) {
			cudaUpdateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
		}

		int currentIdx = time%(MAX_DELAY+1);

		int copySize = 0;
		copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
		copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (totalNeuronNum*currentIdx), copySize);
		if (lif_idx > 0) {
			copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[lif_idx+1]-c_pGpuNet->neuronNums[lif_idx]);
			//copyFromGPU<real>(c_I_syn, c_g_I_syn, c_pGpuNet->synapseNums[exp_idx+1]-c_pGpuNet->synapseNums[exp_idx]);
		}

		update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(c_pGpuNet->pN2SConnection);

		for (int i=0; i<sTypeNum; i++) {
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
					int tmp2 = tmp * network->_node_idx + j;
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

		
		copyToGPU(c_g_cross_id, global_cross_data[dataIdx]._fired_n_idxs, global_cross_data[dataIdx]._fired_n_num);
		addCrossNeurons(c_g_cross_id, global_cross_data[dataIdx]._fired_n_num);

		fprintf(logFile, "Cycle %d: ", time);
		for (int i=0; i<copySize; i++) {
			fprintf(logFile, "%d ", buffers->c_neuronsFired[i]);
		}
		fprintf(logFile, "\n");

		fprintf(dataFile, "Cycle %d: ", time);
		for (int i=0; i<c_pGpuNet->neuronNums[2] - c_pGpuNet->neuronNums[1]; i++) {
			if (i ==  0) {
				fprintf(dataFile, "%lf", c_vm[i]);
			} else {
				fprintf(dataFile, ", %lf", c_vm[i]);
			}
		}
		//for (int i=0; i<c_pGpuNet->synapseNums[1] - c_pGpuNet->synapseNums[0]; i++) {
		//		fprintf(dataFile, ", %lf", c_I_syn[i]);
		//}
		fprintf(dataFile, "\n");

		//copyFromGPU<int>(buffers->c_synapsesFired, buffers->c_gSynapsesLogTable, totalSynapseNum);

		//int synapseCount = 0;
		//if (time > 0) {
		//	for (int i=0; i<totalSynapseNum; i++) {
		//		if (buffers->c_synapsesFired[i] == time) {
		//			if (synapseCount ==  0) {
		//				if (copySize > 0) {
		//					fprintf(logFile, ", ");
		//				}
		//				fprintf(logFile, "%d", i);
		//				synapseCount++;
		//			} else {
		//				fprintf(logFile, ", %d", i);
		//			}
		//		}
		//	}
		//	fprintf(logFile, "\n");
		//}

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

	fclose(logFile);
	fclose(dataFile);

	free_buffers(buffers);
	freeGPUNetwork(c_pGpuNet);

	return NULL;
}
