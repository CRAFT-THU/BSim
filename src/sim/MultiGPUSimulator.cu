/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <iostream>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
// #include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/runtime.h"
#include "../gpu_utils/GBuffers.h"
#include "../net/MultiNetwork.h"

#include "MultiGPUSimulator.h"

using std::cout;
using std::endl;

pthread_barrier_t gpuCycleBarrier;

CrossNodeDataGPU * gCrossDataGPU;

MultiGPUSimulator::MultiGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

MultiGPUSimulator::~MultiGPUSimulator()
{
}

void *run_thread_gpu(void *para);

int MultiGPUSimulator::run(real time, FireInfo &log)
{
	int sim_cycle = round(time/dt);
	reset();

	int device_count = 4;
	checkCudaErrors(cudaGetDeviceCount(&device_count));
	assert(device_count != 0);
	for (int i=0; i<device_count; i++) {
		for (int j=0; j<device_count; j++) {
			if (i!=j) {
				int access = 0;
				checkCudaErrors(cudaDeviceCanAccessPeer(&access, i, j));
				if (access == 1) {
					checkCudaErrors(cudaSetDevice(i));
					checkCudaErrors(cudaDeviceEnablePeerAccess(j, 0));
				}
			}
		}
	}
	checkCudaErrors(cudaSetDevice(0));

	pthread_barrier_init(&gpuCycleBarrier, NULL, device_count);

	MultiNetwork multiNet(network, device_count);
	DistriNetwork *node_nets = multiNet.buildNetworks();
	assert(node_nets != NULL);
	gCrossDataGPU = multiNet.arrangeCrossNodeDataGPU(device_count);
	assert(gCrossDataGPU != NULL);

	pthread_t *thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * device_count);
	assert(thread_ids != NULL);


	for (int i=0; i<device_count; i++) {
		node_nets[i]._sim_cycle = sim_cycle;
		node_nets[i]._node_idx = i;
		node_nets[i]._node_num = device_count;
		node_nets[i]._dt = dt;


		int ret = pthread_create(&(thread_ids[i]), NULL, &run_thread_gpu, (void*)&(node_nets[i]));
		assert(ret == 0);
	}

	for (int i=0; i<device_count; i++) {
		pthread_join(thread_ids[i], NULL);
	}

	pthread_barrier_destroy(&gpuCycleBarrier);

	return 0;
}

void * run_thread_gpu(void *para) {
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

	int nTypeNum = c_pGpuNet->nTypeNum;
	int sTypeNum = c_pGpuNet->sTypeNum;
	int nodeNeuronNum = c_pGpuNet->neuronNums[nTypeNum];
	int allNeuronNum = pCpuNet->pN2SConnection->n_num;
	int nodeSynapseNum = c_pGpuNet->synapseNums[sTypeNum];
	printf("Thread %d NeuronTypeNum: %d, SynapseTypeNum: %d\n", network->_node_idx, nTypeNum, sTypeNum);
	printf("Thread %d NeuronNum: %d, SynapseNum: %d\n", network->_node_idx, nodeNeuronNum, nodeSynapseNum);

	//int dataOffset = network->_node_idx * network->_node_num;
	//int dataIdx = network->_node_idx * network->_node_num + network->_node_idx;

	int MAX_DELAY = c_pGpuNet->MAX_DELAY;
	printf("Thread %d MAX_DELAY: %d\n", network->_node_idx, c_pGpuNet->MAX_DELAY);

	// init_connection<<<1, 1>>>(c_pGpuNet->pN2SConnection);

	GBuffers *buffers = alloc_buffers(allNeuronNum, nodeSynapseNum, MAX_DELAY, network->_dt);

	BlockSize *updateSize = getBlockSize(allNeuronNum, nodeSynapseNum);

#ifdef LOG_DATA
	real *c_vm = hostMalloc<real>(nodeNeuronNum);
	int life_idx = getIndex(c_pGpuNet->nTypes, nTypeNum, LIF);
	int copy_idx = -1;
	real *c_g_vm = NULL;

	if (life_idx >= 0) {
		GLIFNeurons *c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGpuNet->pNeurons[life_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
		copy_idx = life_idx;
	} else {
	}
#endif

	for (int i=0; i<nTypeNum; i++) {
		cout << "Thread " << network->_node_idx << " " << c_pGpuNet->nTypes[i] << ": <<<" << updateSize[c_pGpuNet->nTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->nTypes[i]].blockSize << ">>>" << endl;
	}
	for (int i=0; i<sTypeNum; i++) {
		cout << "Thread " << network->_node_idx << " " << c_pGpuNet->sTypes[i] << ": <<<" << updateSize[c_pGpuNet->sTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->sTypes[i]].blockSize << ">>>" << endl;
	}

	//int * c_g_cross_id = gpuMalloc<int>(global_cross_data[dataIdx]._max_n_num); 

	int * c_g_idx2index = copyToGPU<int>(network->_crossnode_map->_idx2index, allNeuronNum);
	int * c_g_cross_index2idx = copyToGPU<int>(network->_crossnode_map->_crossnode_index2idx, network->_crossnode_map->_cross_size);
	int * c_g_global_cross_data = gpuMalloc<int>(allNeuronNum * network->_node_num);
	int * c_g_fired_n_num = gpuMalloc<int>(network->_node_num);

	vector<int> firedInfo;
	struct timeval ts, te;
	//struct timeval t0, t1, t2, t3, t4, t5,/* t6,*/ t7, t8, t9;
	//double barrier1_time = 0, gpu_cpy_time = 0, peer_cpy_time = 0, barrier2_time=0, copy_time = 0;
	gettimeofday(&ts, NULL);
	for (int time=0; time<network->_sim_cycle; time++) {
		update_time<<<1, 1>>>(time, buffers->c_gFiredTableSizes);

		for (int i=0; i<nTypeNum; i++) {
			assert(c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i] > 0);
			cudaUpdateNeuron[c_pGpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], time, &updateSize[c_pGpuNet->nTypes[i]]);
		}

		//gettimeofday(&t0, NULL);
		pthread_barrier_wait(&gpuCycleBarrier);
		//gettimeofday(&t1, NULL);
		//barrier1_time += (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec)/1000000.0;
		cudaMemset(c_g_fired_n_num, 0, sizeof(int)*network->_node_num);
		//cudaDeviceSynchronize();
		//gettimeofday(&t2, NULL);
		//cudaDeliverNeurons(c_g_idx2index, c_g_cross_index2idx, c_g_global_cross_data, c_g_fired_n_num, network->_node_num, allNeuronNum);
		//for (int i=0; i<network->_node_num; i++) {
		//	int offset = i * network->_node_num + network->_node_idx; 
		//	copyFromGPU<int>(&(global_cross_data[offset]._fired_n_num), c_g_fired_n_num + i, 1);
		//	if (global_cross_data[offset]._fired_n_num > 0) {
		//		copyFromGPU<int>(global_cross_data[offset]._fired_n_idxs, c_g_global_cross_data + allNeuronNum * i, global_cross_data[offset]._fired_n_num);
		//	}
		//}

		cudaDeliverNeurons<<<(allNeuronNum+MAX_BLOCK_SIZE-1)/MAX_BLOCK_SIZE, MAX_BLOCK_SIZE>>>(buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_g_idx2index, c_g_cross_index2idx, c_g_global_cross_data, c_g_fired_n_num, network->_node_num, time);

		checkCudaErrors(cudaMemcpy(gCrossDataGPU->_fired_num + network->_node_idx * network->_node_num, c_g_fired_n_num, sizeof(int)*network->_node_num, cudaMemcpyDeviceToHost));
		//gettimeofday(&t3, NULL);

		for (int i=0; i< network->_node_num; i++) {
			int idx2i = network->_node_idx * network->_node_num + i;
			assert(gCrossDataGPU->_fired_num[idx2i] <= gCrossDataGPU->_max_num[idx2i]);
			if (gCrossDataGPU->_fired_num[idx2i] > 0) {
				checkCudaErrors(cudaMemcpyPeer(gCrossDataGPU->_fired_arrays[idx2i], i, c_g_global_cross_data + allNeuronNum * i, network->_node_idx, gCrossDataGPU->_fired_num[idx2i] * sizeof(int)));
			}
		}
		//gettimeofday(&t7, NULL);

		//gpu_cpy_time += (t3.tv_sec - t2.tv_sec) + (t3.tv_usec - t2.tv_usec)/1000000.0;
		//peer_cpy_time += (t7.tv_sec - t3.tv_sec) + (t7.tv_usec - t3.tv_usec)/1000000.0;

#ifdef LOG_DATA
		int currentIdx = time%(MAX_DELAY+1);

		int copySize = 0;
		copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
		if (copySize > 0) {
			copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (allNeuronNum*currentIdx), copySize);
		}

		if (copy_idx >= 0 && (c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]) > 0) {
			copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
		}
#endif

		for (int i=0; i<sTypeNum; i++) {
			assert(c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i] > 0);
			cudaUpdateSynapse[c_pGpuNet->sTypes[i]](c_pGpuNet->pN2SConnection, c_pGpuNet->pSynapses[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], time, &updateSize[c_pGpuNet->sTypes[i]]);
		}
		//cudaDeviceSynchronize();

		//gettimeofday(&t4, NULL);
		pthread_barrier_wait(&gpuCycleBarrier);
		//gettimeofday(&t5, NULL);
		//barrier2_time += (t5.tv_sec - t4.tv_sec) + (t5.tv_usec - t4.tv_usec)/1000000.0;

		//gettimeofday(&t6, NULL);
		//collectNeurons();
		//gettimeofday(&t7, NULL);
		//cpu_cpy_time += (t7.tv_sec - t6.tv_sec) + (t7.tv_usec - t6.tv_usec)/1000000.0;
		
		//gettimeofday(&t8, NULL);
		//if (global_cross_data[dataIdx]._fired_n_num > 0) {
		//	copyToGPU(c_g_cross_id, global_cross_data[dataIdx]._fired_n_idxs, global_cross_data[dataIdx]._fired_n_num);
		//	addCrossNeurons(c_g_cross_id, global_cross_data[dataIdx]._fired_n_num);
		//}
		for (int i=0; i< network->_node_num; i++) {
			int i2idx = network->_node_idx + network->_node_num * i;
			if (gCrossDataGPU->_fired_num[i2idx] > 0) {
				int num = gCrossDataGPU->_fired_num[i2idx];
				cudaAddCrossNeurons<<<(num+MAX_BLOCK_SIZE-1)/MAX_BLOCK_SIZE, MAX_BLOCK_SIZE>>>(buffers->c_gFiredTable, buffers->c_gFiredTableSizes, gCrossDataGPU->_fired_arrays[i2idx], gCrossDataGPU->_fired_num[i2idx], time);
			}
		}
		
		//gettimeofday(&t9, NULL);
		//copy_time += (t9.tv_sec - t8.tv_sec) + (t9.tv_usec - t8.tv_usec)/1000000.0;

#ifdef LOG_DATA
		for (int i=0; i<copySize; i++) {
			fprintf(log_file, "%d ", buffers->c_neuronsFired[i]);
		}
		fprintf(log_file, "\n");

		for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
			fprintf(v_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(v_file, "\n");
#endif

		//pthread_barrier_wait(&gpuCycleBarrier);
	}
	pthread_barrier_wait(&gpuCycleBarrier);
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
	//printf("Thread %d cost : barrier1 %lf, DtoH %lf, DtoD %lf, barrier2 %lf, HtoD %lf\n", network->_node_idx, barrier1_time, gpu_cpy_time, peer_cpy_time, barrier2_time, copy_time);

	int *rate = (int*)malloc(sizeof(int)*nodeNeuronNum);
	copyFromGPU<int>(rate, buffers->c_gFireCount, nodeNeuronNum);

	char fire_filename[512];
	sprintf(fire_filename, "GFire_%d.log", network->_node_idx); 
	FILE *rate_file = fopen(fire_filename, "w+");
	if (rate_file == NULL) {
		printf("Open file Sim.log failed\n");
		return NULL;
	}

	for (int i=0; i<nodeNeuronNum; i++) {
		fprintf(rate_file, "%d \t", rate[i]);
	}

	free(rate);
	fclose(rate_file);

	fclose(log_file);
	fclose(v_file);

	free_buffers(buffers);
	freeGPUNetwork(c_pGpuNet);

	return NULL;
}

