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

struct gpu_buffers {
	// Arrays
	int *gTimeTable;

	// Neuron Arrays
	real *gNeuronInput;

	// Neuron Tables
	int *gFiredTable;
	int *gFiredTableSizes;

	int *gActiveTable;
	int gActiveTableSize;

	// Synapse Tables
	int *gSynapsesActiveTable;
	int *gSynapsesActiveTableSize;

	int *gSynapsesLogTable;
}

int global_malloc()
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

	GNetwork *c_pGpuNet = copyDataToGPU(pCpuNet);

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = pCpuNet->MAX_DELAY;
	printf("MAX_DELAY: %lf %lf %d\n", network->maxDelay, dt, pCpuNet->MAX_DELAY);

	int * c_n_fired = (int*)malloc(sizeof(int)*((totalNeuronNum)));
	bool * c_s_fired = (bool*)malloc(sizeof(bool)*((totalSynapseNum)));
	real * c_n_input = (real*)malloc(sizeof(real)*((totalNeuronNum)));

	checkCudaErrors(cudaMalloc((void**)&c_gTimeTable, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMemset(c_gTimeTable, 0, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMalloc((void**)&c_gFiredTable, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMemset(c_gFiredTable, 0, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum)));
	checkCudaErrors(cudaMemset(c_gSynapsesFiredTable, 0, sizeof(bool)*(totalSynapseNum)));
	checkCudaErrors(cudaMalloc((void**)&c_gNeuronInput, sizeof(real)*(totalNeuronNum)));
	checkCudaErrors(cudaMemset(c_gNeuronInput, 0, sizeof(real)*(totalNeuronNum)));

	init_global<<<1, 1, 0>>>(MAX_DELAY, c_gTimeTable, c_gNeuronInput, c_gFiredTable, totalNeuronNum, c_gSynapsesFiredTable, totalSynapseNum, pGpuNet);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
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
		fflush(stdout);
		//update_pre_neuron<<<1, 1, 0>>>(pGpuNet, simTime);
		//update_lif_neuron<<<3, 2, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, c_pGpuNet->neuronNum, time);

		checkCudaErrors(cudaMemcpy(c_n_input, c_gNeuronInput, sizeof(real)*(totalNeuronNum), cudaMemcpyDeviceToHost));
		for (int i=0; i<nTypeNum; i++) {
			updateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], time, &updateSize[c_pGpuNet->nTypes[i]]);
		}

		//update_pre_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(pGpuNet, time);

		checkCudaErrors(cudaMemcpy(c_s_fired, c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum), cudaMemcpyDeviceToHost));

		//update_exp_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);
		for (int i=0; i<sTypeNum; i++) {
			updateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], time, &updateSize[pCpuNet->nTypes[i]]);
		}

		update_post_synapse<<<postSize.gridSize, postSize.blockSize>>>(pGpuNet, time);

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

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));

	freeGPUData(c_pGpuNet);
	checkCudaErrors(cudaFree(pGpuNet));

	return 0;
}

