/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>

#include "../third_party/cuda/helper_cuda.h"
#include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/gpu_func.h"
#include "../gpu_utils/gpu_kernel.h"
#include "SingleGPUSimulator.h"

SingleGPUSimulator::SingleGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

SingleGPUSimulator::~SingleGPUSimulator()
{
}

int SingleGPUSimulator::run(real time)
{
	findCudaDevice(0, NULL);

	int sim_cycle = round(time/dt);

	reset();

	GNetwork *pCpuNet = network->buildNetwork();

	FILE *logFile = fopen("GSim.log", "w+");
	if (logFile == NULL) {
		printf("ERROR: Open file SimGPU.log failed\n");
		return -1;
	}
	FILE *dataFile = fopen("GSim.data", "w+");
	if (dataFile == NULL) {
		printf("ERROR: Open file SimGPU.log failed\n");
		return -1;
	}

	GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = pCpuNet->MAX_DELAY;
	printf("MAX_DELAY: %lf %lf %d\n", network->maxDelay, dt, pCpuNet->MAX_DELAY);

	// Arrays
	//int *c_gTimeTable;
	// Neuron Arrays
	real *c_gNeuronInput;
	// Neuron Tables
	int *c_gFiredTable;
	int *c_gFiredTableSizes;
	int *c_gActiveTable;
	// Synapse Tables
	int *c_gSynapsesActiveTable;
	int *c_gSynapsesLogTable;

	//checkCudaErrors(cudaMalloc((void**)&c_gTimeTable, sizeof(int)*(MAX_DELAY+1)));
	//checkCudaErrors(cudaMemset(c_gTimeTable, 0, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMalloc((void**)&c_gNeuronInput, sizeof(real)*(totalNeuronNum)));
	checkCudaErrors(cudaMemset(c_gNeuronInput, 0, sizeof(real)*(totalNeuronNum)));

	checkCudaErrors(cudaMalloc((void**)&c_gFiredTable, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMemset(c_gFiredTable, 0, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));

	checkCudaErrors(cudaMalloc((void**)&c_gFiredTableSizes, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMemset(c_gFiredTableSizes, 0, sizeof(int)*(MAX_DELAY+1)));

	checkCudaErrors(cudaMalloc((void**)&c_gActiveTable, sizeof(int)*(totalNeuronNum)));
	checkCudaErrors(cudaMemset(c_gActiveTable, 0, sizeof(int)*(totalNeuronNum)));

	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesActiveTable, sizeof(int)*(totalSynapseNum)));
	checkCudaErrors(cudaMemset(c_gSynapsesActiveTable, 0, sizeof(int)*(totalSynapseNum)));
	
	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesLogTable, sizeof(int)*(totalSynapseNum)));
	checkCudaErrors(cudaMemset(c_gSynapsesLogTable, 0, sizeof(int)*(totalSynapseNum)));

	int timeTableCap = MAX_DELAY+1;
	checkCudaErrors(cudaMemcpyToSymbol(&MAX_DELAY, &MAX_DELAY, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(&gTimeTableCap, &timeTableCap, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(&gFiredTableCap, &totalNeuronNum, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(&gSynapsesTableCap, &totalSynapseNum, sizeof(int)));

	init_buffers<<<1, 1, 0>>>(/*c_gTimeTable,*/ c_gNeuronInput, c_gFiredTable, c_gFiredTableSizes, c_gActiveTable, c_gSynapsesActiveTable, c_gSynapsesLogTable);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
	BlockSize preSize = { 0, 0, 0};
	BlockSize postSize = { 0, 0, 0};
	cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_lif_neuron, 0, totalNeuronNum); 
	preSize.gridSize = (totalNeuronNum + (preSize.blockSize) - 1) / (preSize.blockSize);
	cudaOccupancyMaxPotentialBlockSize(&(postSize.minGridSize), &(postSize.blockSize), update_lif_neuron, 0, totalSynapseNum); 
	postSize.gridSize = (totalSynapseNum + (postSize.blockSize) - 1) / (postSize.blockSize);

	int * c_n_fired = (int*)malloc(sizeof(int)*((totalNeuronNum)));
	int * c_s_fired = (int*)malloc(sizeof(int)*((totalSynapseNum)));
	//real * c_n_input = (real*)malloc(sizeof(real)*((totalNeuronNum)));

	vector<int> firedInfo;
	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);
		fflush(stdout);

		//checkCudaErrors(cudaMemcpy(c_n_input, c_gNeuronInput, sizeof(real)*(totalNeuronNum), cudaMemcpyDeviceToHost));
		for (int i=0; i<nTypeNum; i++) {
			updateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
		}

		update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(c_pGpuNet->pN2SConnection);

		for (int i=0; i<sTypeNum; i++) {
			updateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], &updateSize[pCpuNet->nTypes[i]]);
		}

		update_time<<<1, 1>>>();

		int currentIdx = time%(MAX_DELAY+1);
		checkCudaErrors(cudaMemcpy(c_n_fired, c_gFiredTable + totalNeuronNum*currentIdx, sizeof(int)*(totalNeuronNum), cudaMemcpyDeviceToHost));

		fprintf(logFile, "Cycle %d: ", time);
		firedInfo.clear();
		for (int i=0; i<pCpuNet->neuronNums[nTypeNum]; i++) {
			if (c_n_fired[i] > 0) {
				firedInfo.push_back(i);
			}
		}
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
				fprintf(logFile, ", ");
			}
			fprintf(logFile, "%d_%d", network->idx2sid[firedInfo[0]].groupId, network->idx2sid[firedInfo[0]].id);
			for (int i=1; i<size2; i++) {
				fprintf(logFile, ", %d_%d", network->idx2sid[firedInfo[i]].groupId, network->idx2sid[firedInfo[i]].id);
			}
		}
		fprintf(logFile, "\n");
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

	fclose(logFile);
	fclose(dataFile);

	//checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gNeuronInput));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gActiveTable));
	checkCudaErrors(cudaFree(c_gSynapsesActiveTable));
	checkCudaErrors(cudaFree(c_gSynapsesLogTable));

	freeGPUNetwork(c_pGpuNet);

	return 0;
}

