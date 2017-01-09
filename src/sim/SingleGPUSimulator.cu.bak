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
	GNetwork *pGpuNet;
	void **c_pNeurons;
	void **c_pSynapses;
	int *c_gTimeTable = NULL;
	int *c_gFiredTable = NULL;
	bool *c_gSynapsesFiredTable = NULL;
	real *c_gNeuronInput = NULL;

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
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
			updateType[pCpuNet->nTypes[i]](c_pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i], time, &updateSize[pCpuNet->nTypes[i]]);
		}

		//update_pre_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(pGpuNet, time);

		checkCudaErrors(cudaMemcpy(c_s_fired, c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum), cudaMemcpyDeviceToHost));

		//update_exp_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);
		for (int i=0; i<sTypeNum; i++) {
			updateType[pCpuNet->sTypes[i]](c_pSynapses[i], pCpuNet->synapseNums[i+1]-pCpuNet->synapseNums[i], time, &updateSize[pCpuNet->nTypes[i]]);
		}

		update_post_synapse<<<postSize.gridSize, postSize.blockSize>>>(pGpuNet, time);

		int currentIdx = time%(MAX_DELAY+1);
		checkCudaErrors(cudaMemcpy(c_n_fired, c_gFiredTable + totalNeuronNum*currentIdx, sizeof(int)*(totalNeuronNum), cudaMemcpyDeviceToHost));

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

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));

	freeGPUData(c_pGpuNet);
	checkCudaErrors(cudaFree(pGpuNet));

	return 0;
}

