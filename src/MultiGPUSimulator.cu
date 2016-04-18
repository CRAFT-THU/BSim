/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <mpi.h>
#include <sys/time.h>
#include <stdio.h>

#include "utils/cuda/helper_cuda.h"
#include "gpu_utils.h"
#include "gpu_func.h"
#include "gpu_kernel.h"
#include "MultiGPUSimulator.h"

#define TEST

MultiGPUSimulator::MultiGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
	rank = -1;
	size = -1;
}

MultiGPUSimulator::~MultiGPUSimulator()
{
}

GNetwork* getGlobalData(GNetwork *, int, int);
GNetwork* splitNetwork(GNetwork *, GNetwork *, int, int);

int MultiGPUSimulator::init(int argc, char**argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	return size;
}

int MultiGPUSimulator::run(real time)
{
	findCudaDevice(0, NULL);

	int sim_cycle = round(time/dt);

	reset();

	GNetwork *pCpuNet = NULL;
	GNetwork *pAllNet = NULL;
	FILE *logFile = NULL;
	FILE *dataFile = NULL;
	if (rank == 0) {
		logFile = fopen("MGSim.log", "w+");
		if (logFile == NULL) {
			printf("ERROR: Open file MGSim.log failed\n");
			return -1;
		}
		dataFile = fopen("MGSim.data", "w+");
		if (dataFile == NULL) {
			printf("ERROR: Open file MGSim.log failed\n");
			return -1;
		}

		pAllNet = network->buildNetwork();
	}

	pCpuNet = getGlobalData(pAllNet, size, rank);
	MPI_Bcast(pCpuNet, sizeof(GNetwork), MPI_BYTE, 0, MPI_COMM_WORLD);
	MPI_Bcast(pCpuNet->nTypes, sizeof(Type)*pCpuNet->nTypeNum, MPI_BYTE, 0, MPI_COMM_WORLD);
	MPI_Bcast(pCpuNet->sTypes, sizeof(Type)*pCpuNet->sTypeNum, MPI_BYTE, 0, MPI_COMM_WORLD);
	MPI_Bcast(pCpuNet->gNeuronNums, sizeof(int)*(pCpuNet->nTypeNum+1), MPI_BYTE, 0, MPI_COMM_WORLD);
	MPI_Bcast(pCpuNet->gSynapseNums, sizeof(int)*(pCpuNet->sTypeNum+1), MPI_BYTE, 0, MPI_COMM_WORLD);
	splitNetwork(pCpuNet, pAllNet, size, rank);


	GNetwork *c_pGpuNet = copyDataToGPU(pCpuNet);
	GNetwork *pGpuNet;
	void **c_pNeurons;
	void **c_pSynapses;
	int *c_gTimeTable = NULL;
	int *c_gFiredTable = NULL;
	bool *c_gSynapsesFiredTable = NULL;
	real *c_gNeuronInput = NULL;

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int thisNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int thisSynapseNum = pCpuNet->synapseNums[sTypeNum];
	int totalNeuronNum = pCpuNet->gNeuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->gSynapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", thisNeuronNum, thisSynapseNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = (int)(pCpuNet->MAX_DELAY/dt);
	printf("MAX_DELAY: %lf %lf %lf\n", network->maxDelay, pCpuNet->MAX_DELAY, dt);
	printf("MAX_DELAY: %u\n", MAX_DELAY);

	int * c_n_fired = (int*)malloc(sizeof(int)*((totalNeuronNum)));
	bool * c_s_fired = (bool*)malloc(sizeof(bool)*((totalSynapseNum)));
	real * c_n_input = (real*)malloc(sizeof(real)*((totalNeuronNum)));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNet), sizeof(GNetwork)));
	checkCudaErrors(cudaMemcpy(pGpuNet, c_pGpuNet, sizeof(GNetwork), cudaMemcpyHostToDevice));

	c_pNeurons = (void**)malloc(sizeof(void*)*pCpuNet->nTypeNum);
	checkCudaErrors(cudaMemcpy(c_pNeurons, c_pGpuNet->pNeurons, sizeof(void*)*(pCpuNet->nTypeNum), cudaMemcpyDeviceToHost));
	c_pSynapses = (void**)malloc(sizeof(void*)*pCpuNet->sTypeNum);
	checkCudaErrors(cudaMemcpy(c_pSynapses, c_pGpuNet->pSynapses, sizeof(void*)*(pCpuNet->sTypeNum), cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMalloc((void**)&c_gTimeTable, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMemset(c_gTimeTable, 0, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMalloc((void**)&c_gFiredTable, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMemset(c_gFiredTable, 0, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum)));
	checkCudaErrors(cudaMemset(c_gSynapsesFiredTable, 0, sizeof(bool)*(totalSynapseNum)));
	checkCudaErrors(cudaMalloc((void**)&c_gNeuronInput, sizeof(real)*(totalNeuronNum)));
	checkCudaErrors(cudaMemset(c_gNeuronInput, 0, sizeof(real)*(totalNeuronNum)));

	init_global<<<1, 1, 0>>>(MAX_DELAY, c_gTimeTable, c_gNeuronInput, c_gFiredTable, totalNeuronNum, c_gSynapsesFiredTable, totalSynapseNum, pGpuNet);

	BlockSize *updateSize = getBlockSize(thisNeuronNum, thisSynapseNum);
	BlockSize preSize = { 0, 0, 0};
	BlockSize postSize = { 0, 0, 0};
	cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_lif_neuron, 0, totalNeuronNum); 
	preSize.gridSize = (totalNeuronNum + (preSize.blockSize) - 1) / (preSize.blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(postSize.minGridSize), &(postSize.blockSize), update_lif_neuron, 0, totalSynapseNum); 
	postSize.gridSize = (totalSynapseNum + (postSize.blockSize) - 1) / (postSize.blockSize);

	vector<int> firedInfo;
	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);

		checkCudaErrors(cudaMemcpy(c_n_input, c_gNeuronInput, sizeof(real)*(totalNeuronNum), cudaMemcpyDeviceToHost));
		MPI_Allreduce(MPI_IN_PLACE, c_n_input, totalNeuronNum, MPI_CREAL, MPI_SUM, MPI_COMM_WORLD);
		checkCudaErrors(cudaMemcpy(c_gNeuronInput, c_n_input, sizeof(real)*(totalNeuronNum), cudaMemcpyHostToDevice));

		for (int i=0; i<nTypeNum; i++) {
			updateType[pCpuNet->nTypes[i]](c_pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i], time, &updateSize[pCpuNet->nTypes[i]]);
		}

		update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(pGpuNet, time);

		checkCudaErrors(cudaMemcpy(c_s_fired, c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum), cudaMemcpyDeviceToHost));
		MPI_Allreduce(MPI_IN_PLACE, c_s_fired, sizeof(bool)*totalSynapseNum, MPI_BYTE, MPI_SUM, MPI_COMM_WORLD);
		checkCudaErrors(cudaMemcpy(c_gSynapsesFiredTable, c_s_fired, sizeof(bool)*(totalSynapseNum), cudaMemcpyHostToDevice));

		for (int i=0; i<sTypeNum; i++) {
			updateType[pCpuNet->sTypes[i]](c_pSynapses[i], pCpuNet->synapseNums[i+1]-pCpuNet->synapseNums[i], time, &updateSize[pCpuNet->nTypes[i]]);
		}

		update_post_synapse<<<postSize.gridSize, postSize.blockSize>>>(pGpuNet, time);

		int currentIdx = time%(MAX_DELAY+1);
		checkCudaErrors(cudaMemcpy(c_n_fired, c_gFiredTable + totalNeuronNum*currentIdx, sizeof(int)*(totalNeuronNum), cudaMemcpyDeviceToHost));
		MPI_Allreduce(MPI_IN_PLACE, c_n_fired, totalNeuronNum, MPI_CREAL, MPI_SUM, MPI_COMM_WORLD);

		//int count = 0;
		fprintf(dataFile, "%d", time);
		for (int i=0; i<pCpuNet->neuronNums[nTypeNum]; i++) {
			fprintf(dataFile, ", %lf", c_n_input[i]);
		}
		fprintf(dataFile, "\n");

		fprintf(logFile, "Cycle %d: ", time);
		firedInfo.clear();
		for (int i=0; i<pCpuNet->neuronNums[nTypeNum]; i++) {
			if (c_n_fired[i] > 0) {
				firedInfo.push_back(i);
			}
		}

		if (rank == 0) {
			int size = firedInfo.size();
			if (size > 0) {
				fprintf(logFile, "%d_%d", network->idx2nid[firedInfo[0]].groupId, network->idx2nid[firedInfo[0]].id);
				for (int i=1; i<size; i++) {
					fprintf(logFile, ", %d_%d", network->idx2nid[firedInfo[i]].groupId, network->idx2nid[firedInfo[i]].id);
				}
			}
			firedInfo.clear();
			for (int i=0; i<pCpuNet->synapseNums[sTypeNum]; i++) {
				if (c_s_fired[i]) {
					firedInfo.push_back(i);
				}
			}
			int size2 = firedInfo.size();
			if (size2 > 0) {
				if (size > 0) {
					printf(", ");
				}
				fprintf(logFile, "%d_%d", network->idx2sid[firedInfo[0]].groupId, network->idx2sid[firedInfo[0]].id);
				for (int i=1; i<size2; i++) {
					fprintf(logFile, ", %d_%d", network->idx2sid[firedInfo[i]].groupId, network->idx2sid[firedInfo[i]].id);
				}
			}
			fprintf(logFile, "\n");
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

	printf("\nSimulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	if (rank == 0) {
		fclose(logFile);
		fclose(dataFile);
	}

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));

	freeGPUData(c_pGpuNet);
	checkCudaErrors(cudaFree(pGpuNet));

	return 0;
}

GNetwork* getGlobalData(GNetwork *network, int rankSize, int rank)
{
	int nTypeNum = network->nTypeNum;
	int sTypeNum = network->sTypeNum;

	GNetwork *ret = (GNetwork *)malloc(sizeof(GNetwork));
	memcpy(ret, network, sizeof(GNetwork));

	ret->pNeurons = (void**)malloc(sizeof(void*)*nTypeNum);
	ret->pSynapses = (void**)malloc(sizeof(void*)*sTypeNum);
	ret->nOffsets = (int*)malloc(sizeof(int)*(nTypeNum));
	ret->sOffsets = (int*)malloc(sizeof(int)*(sTypeNum));
	ret->neuronNums = (int*)malloc(sizeof(int)*(nTypeNum + 1));
	ret->synapseNums = (int*)malloc(sizeof(int)*(sTypeNum + 1));
	ret->neuronNums[0] = 0;
	ret->synapseNums[0] = 0;

	ret->nTypes = (Type*)malloc(sizeof(Type)*nTypeNum);
	ret->sTypes = (Type*)malloc(sizeof(Type)*sTypeNum);
	ret->gNeuronNums = (int*)malloc(sizeof(int)*(nTypeNum + 1));
	ret->gSynapseNums = (int*)malloc(sizeof(int)*(sTypeNum + 1));

	if (rank == 0) {
		memcpy(ret->nTypes, network->nTypes, sizeof(Type)*nTypeNum);
		memcpy(ret->sTypes, network->sTypes, sizeof(Type)*sTypeNum);
		memcpy(ret->gNeuronNums, network->gNeuronNums, sizeof(int)*(nTypeNum+1));
		memcpy(ret->gSynapseNums, network->gSynapseNums, sizeof(int)*(sTypeNum+1));
	}

	for (int i=0; i<nTypeNum; i++) {
		int num_i = network->gNeuronNums[i+1] - network->gNeuronNums[i];
		int size = num_i/rankSize;
		int range = num_i%size;

		int size_tt = 0;
		if (range > 0) {
			size_tt = size+1;
		} else {
			size_tt = size;
		}

		network->neuronNums[i+1] = network->neuronNums[i] + size_tt;
	}
	for (int i=0; i<sTypeNum; i++) {
		int num_i = network->gSynapseNums[i+1] - network->gSynapseNums[i];
		int size = num_i/rankSize;
		int range = num_i%size;

		int size_tt = 0;
		if (range > 0) {
			size_tt = size+1;
		} else {
			size_tt = size;
		}

		network->synapseNums[i+1] = network->synapseNums[i] + size_tt;
	}


	return ret;
}

GNetwork* splitNetwork(GNetwork *network, GNetwork *allNet, int rankSize, int rank)
{
	int nTypeNum = network->nTypeNum;
	int sTypeNum = network->sTypeNum;
	int totalNeuronNum = network->gNeuronNums[nTypeNum];
	int totalSynapseNum = network->gSynapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);
	printf("MAX_DELAY: %lf\n", network->MAX_DELAY);

	if (rank == 0) {
		for (int i=0; i<nTypeNum; i++) {
			int size = network->neuronNums[i+1] - network->neuronNums[i];

			network->nOffsets[i] = 0;
			//Copy neurons
			//copyNeurons[i](network, allNet, i, size);
		}
		for (int i=0; i<sTypeNum; i++) {
			int size = network->neuronNums[i+1] - network->neuronNums[i];

			network->sOffsets[i] = 0;
			//Copy neurons
			//copySynapse[i](network, allNet, i, size);
		}
	} else {
		for (int i=0; i<nTypeNum; i++) {
			int num_i = network->gNeuronNums[i+1] - network->gNeuronNums[i];
			int size = num_i/rankSize;
			int range = num_i%size;

			if (i < range) {
				network->nOffsets[i] = (size+1)*range;
			} else {
				network->nOffsets[i] = (size+1)*range + size*(i-range);
			}
			//Copy neurons
			//mpiSendNeurons(network, allNet, i, size);
		}
		for (int i=0; i<sTypeNum; i++) {
			int num_i = network->gNeuronNums[i+1] - network->gNeuronNums[i];
			int size = num_i/rankSize;
			int range = num_i%size;

			if (i < range) {
				network->nOffsets[i] = (size+1)*range;
			} else {
				network->nOffsets[i] = (size+1)*range + size*(i-range);
			}
			//Copy neurons
			//mpiSendSynapse(network, allNet, i, size);
		}
	}

	return network;
}
