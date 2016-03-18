/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <stdio.h>

#include "utils/cuda/helper_cuda.h"
#include "GFunc.h"
#include "gpu_kernel.h"
#include "SingleGPUSimulator.h"

#define TEST

SingleGPUSimulator::SingleGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

SingleGPUSimulator::~SingleGPUSimulator()
{
}

GNetwork* copyDataToGPU(GNetwork *);
int freeGPUData(GNetwork *);

int SingleGPUSimulator::run(real time)
{
	findCudaDevice(0, NULL);

	int sim_cycle = (int) (time/dt);

	reset();

	GNetwork *pCpuNet = network->buildNetwork();

	FILE *logFile = fopen("SimGPU.log", "w+");
	if (logFile == NULL) {
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

	int * c_n_fired = (int*)malloc(sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1)));
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

	printf("Start runing for %d cycles\n", sim_cycle);
	for (int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);
		//update_pre_neuron<<<1, 1, 0>>>(pGpuNet, simTime);
		//update_lif_neuron<<<3, 2, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, c_pGpuNet->neuronNum, time);
		for (int i=0; i<nTypeNum; i++) {
			updateType[pCpuNet->nTypes[i]](c_pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i], time);
		}

		//update_pre_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_pre_synapse<<<1, 1, 0>>>(pGpuNet, time);
		//update_exp_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);
		for (int i=0; i<sTypeNum; i++) {
			updateType[pCpuNet->sTypes[i]](c_pSynapses[i], pCpuNet->synapseNums[i+1]-pCpuNet->synapseNums[i], time);
		}

		update_post_synapse<<<1, 1, 0>>>(pGpuNet, time);

		int currentIdx = time%(MAX_DELAY+1);
		checkCudaErrors(cudaMemcpy(c_n_fired, c_gFiredTable, sizeof(int)*((totalNeuronNum)*(MAX_DELAY+1)), cudaMemcpyDeviceToHost));

		int count = 0;
		for (int i=0; i<pCpuNet->neuronNums[nTypeNum]; i++) {
			if (c_n_fired[currentIdx*totalNeuronNum+i] > 0) {
				count++;
			}
		}
		if (count > 0) {
			fprintf(logFile, "Cycle %d:", time);
			for (int i=0; i<totalNeuronNum; i++) {
				if (c_n_fired[currentIdx*totalNeuronNum+i] > 0) {
					fprintf(logFile, " %d_%d", network->idx2id[i].groupId, network->idx2id[i].id);
				}
			}
			fprintf(logFile, "\n");
			fflush(stdout);
		}
	}
	printf("\nFinish runing\n");

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));

	freeGPUData(c_pGpuNet);
	checkCudaErrors(cudaFree(pGpuNet));

	return 0;
}

GNetwork* copyDataToGPU(GNetwork *pCpuNet)
{
	GNetwork *tmpNet = (GNetwork*)malloc(sizeof(GNetwork));
	memcpy(tmpNet, pCpuNet, sizeof(GNetwork));

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;

	Type *nTypes, *sTypes;
	checkCudaErrors(cudaMalloc((void**)&(nTypes), sizeof(Type)*nTypeNum));
	checkCudaErrors(cudaMemcpy(nTypes, pCpuNet->nTypes, sizeof(Type)*nTypeNum, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(sTypes), sizeof(Type)*sTypeNum));
	checkCudaErrors(cudaMemcpy(sTypes, pCpuNet->sTypes, sizeof(Type)*sTypeNum, cudaMemcpyHostToDevice));

	int *neuronNums, *synapseNums;
	checkCudaErrors(cudaMalloc((void**)&(neuronNums), sizeof(int)*(nTypeNum+1)));
	checkCudaErrors(cudaMemcpy(neuronNums, pCpuNet->neuronNums, sizeof(int)*(nTypeNum+1), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(synapseNums), sizeof(int)*(sTypeNum+1)));
	checkCudaErrors(cudaMemcpy(synapseNums, pCpuNet->synapseNums, sizeof(int)*(sTypeNum+1), cudaMemcpyHostToDevice));

	//TODO support multitype N and S
	void **pNs = (void**)malloc(sizeof(void*)*nTypeNum);
	void **pSs = (void**)malloc(sizeof(void*)*sTypeNum);

	for (int i=0; i<nTypeNum; i++) {
		void *pNTmp = createType[pCpuNet->nTypes[i]]();
		memcpy(pNTmp, pCpuNet->pNeurons[i], getSize[pCpuNet->nTypes[i]]());
		cudaAllocType[pCpuNet->nTypes[i]](pCpuNet->pNeurons[i], pNTmp);
		void *pNGpu;
		checkCudaErrors(cudaMalloc((void**)&(pNGpu), getSize[pCpuNet->nTypes[i]]()));
		checkCudaErrors(cudaMemcpy(pNGpu, pNTmp, getSize[pCpuNet->nTypes[i]](), cudaMemcpyHostToDevice));
		free(pNTmp);
		pNs[i] = pNGpu;
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pSTmp = createType[pCpuNet->sTypes[i]]();
		memcpy(pSTmp, pCpuNet->pSynapses[i], getSize[pCpuNet->sTypes[i]]());
		cudaAllocType[pCpuNet->sTypes[i]](pCpuNet->pSynapses[i], pSTmp);
		void *pSGpu;
		checkCudaErrors(cudaMalloc((void**)&(pSGpu), getSize[pCpuNet->sTypes[i]]()));
		checkCudaErrors(cudaMemcpy(pSGpu, pSTmp, getSize[pCpuNet->sTypes[i]](), cudaMemcpyHostToDevice));
		free(pSTmp);
		pSs[i] = pSGpu;

	}

	//GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	//memcpy(pNTmp, pN, sizeof(GLIFNeurons));
	//GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	//memcpy(pSTmp, pS, sizeof(GExpSynapses));

	//GLIFNeurons *pNGpu;
	//GExpSynapses *pSGpu;

	//pN->allocGNeurons((GLIFNeurons*)pNTmp);
	//pS->allocGSynapses((GExpSynapses*)pSTmp);
	
	//checkCudaErrors(cudaMalloc((void**)&(pNGpu), sizeof(GLIFNeurons)));
	//checkCudaErrors(cudaMemcpy(pNGpu, pNTmp, sizeof(GLIFNeurons), cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(pSGpu), sizeof(GExpSynapses)));
	//checkCudaErrors(cudaMemcpy(pSGpu, pSTmp, sizeof(GExpSynapses), cudaMemcpyHostToDevice));
	//free(pNTmp);
	//free(pSTmp);

	void **pNeurons, **pSynapses;
	checkCudaErrors(cudaMalloc((void**)&(pNeurons), sizeof(void*)*nTypeNum));
	checkCudaErrors(cudaMemcpy(pNeurons, pNs, sizeof(void*)*nTypeNum, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pSynapses), sizeof(void*)*sTypeNum));
	checkCudaErrors(cudaMemcpy(pSynapses, pSs, sizeof(void*)*sTypeNum, cudaMemcpyHostToDevice));
	free(pNs);
	free(pSs);

	tmpNet->pNeurons = pNeurons;
	tmpNet->pSynapses = pSynapses;
	tmpNet->nTypes = nTypes;
	tmpNet->sTypes = sTypes;
	tmpNet->neuronNums = neuronNums;
	tmpNet->synapseNums = synapseNums;

	return tmpNet;

	//GNetwork *pGpuNet;
	//checkCudaErrors(cudaMalloc((void**)&(pGpuNet), sizeof(GNetwork)));
	//checkCudaErrors(cudaMemcpy(pGpuNet, tmpNet, sizeof(GNetwork), cudaMemcpyHostToDevice));
	//free(tmpNet);

	//return pGpuNet;
}


int freeGPUData(GNetwork *pGpuNet)
{
	GNetwork *pTmpNet = pGpuNet;
	//GNetwork *pTmpNet = (GNetwork*)malloc(sizeof(GNetwork));
	//checkCudaErrors(cudaMemcpy(pTmpNet, pGpuNet, sizeof(GLIFNeurons), cudaMemcpyDeviceToHost));

	int nTypeNum = pTmpNet->nTypeNum;
	int sTypeNum = pTmpNet->sTypeNum;

	void **pTmpNs = (void**)malloc(sizeof(void*) * nTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpNs, pTmpNet->pNeurons, sizeof(void*)*nTypeNum, cudaMemcpyDeviceToHost));
	void **pTmpSs = (void**)malloc(sizeof(void*) * sTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpSs, pTmpNet->pSynapses, sizeof(void*)*sTypeNum, cudaMemcpyDeviceToHost));

	Type * pTmpNT = (Type*)malloc(sizeof(Type)*nTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpNT, pTmpNet->nTypes, sizeof(Type)*nTypeNum, cudaMemcpyDeviceToHost));
	Type * pTmpST = (Type*)malloc(sizeof(Type)*sTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpST, pTmpNet->sTypes, sizeof(Type)*sTypeNum, cudaMemcpyDeviceToHost));

	for (int i=0; i<nTypeNum; i++) {
		void *pTmpN = createType[pTmpNT[i]]();
		checkCudaErrors(cudaMemcpy(pTmpN, pTmpNs[i], getSize[pTmpNT[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpNT[i]](pTmpN);
		free(pTmpN);
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pTmpS = createType[pTmpST[i]]();
		checkCudaErrors(cudaMemcpy(pTmpS, pTmpSs[i], getSize[pTmpST[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpST[i]](pTmpS);
		free(pTmpS);
	}

        //GLIFNeurons *pN = (GLIFNeurons*)pGpuNet->pNeurons;
	//GExpSynapses *pS = (GExpSynapses*)pGpuNet->pSynapses;

	//GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	//GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	//checkCudaErrors(cudaMemcpy(pNTmp, pN, sizeof(GLIFNeurons), cudaMemcpyDeviceToHost));
	//checkCudaErrors(cudaMemcpy(pSTmp, pS, sizeof(GExpSynapses), cudaMemcpyDeviceToHost));

	//freeGNeurons(pNTmp);
	//freeGSynapses(pSTmp);


	checkCudaErrors(cudaFree(pTmpNet->nTypes));
	checkCudaErrors(cudaFree(pTmpNet->sTypes));
	free(pTmpNT);
	free(pTmpST);

	checkCudaErrors(cudaFree(pTmpNet->neuronNums));
	checkCudaErrors(cudaFree(pTmpNet->synapseNums));

	checkCudaErrors(cudaFree(pTmpNet->pNeurons));
	checkCudaErrors(cudaFree(pTmpNet->pSynapses));
	free(pTmpNs);
	free(pTmpSs);
	//checkCudaErrors(cudaFree(pGpuNet));
	free(pTmpNet);

	return 0;
}
