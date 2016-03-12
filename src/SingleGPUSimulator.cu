/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <stdio.h>

#include "utils/cuda/helper_cuda.h"
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
	bool * c_n_fired = (bool*)malloc(sizeof(bool)*pCpuNet->neuronNum);

#ifdef TEST
	real * c_n_vm = (real*)malloc(sizeof(real)*pCpuNet->neuronNum);
	real * c_s_vm = (real*)malloc(sizeof(real)*pCpuNet->synapseNum);
#endif 

	FILE *logFile = fopen("SimGPU.log", "w+");
	if (logFile == NULL) {
		printf("ERROR: Open file SimGPU.log failed\n");
		return -1;
	}

#ifdef TEST
	FILE **fileN = (FILE**)malloc(sizeof(FILE*)*pCpuNet->neuronNum);
	char filename[128];
	for (int i=0; i<pCpuNet->neuronNum; i++) {
		sprintf(filename, "GNeuron_%d.log", i);
		fileN[i] = fopen(filename, "w+");
	}
	FILE **fileS = (FILE**)malloc(sizeof(FILE*)*pCpuNet->synapseNum);
	for (int i=0; i<pCpuNet->synapseNum; i++) {
		sprintf(filename, "GSynapse_%d.log", i);
		fileS[i] = fopen(filename, "w+");
	}
#endif

	GNetwork *c_pGpuNet = copyDataToGPU(pCpuNet);
	int *c_gTimeTable = NULL;
	bool *c_gFiredTable = NULL;
	bool *c_gSynapsesFiredTable = NULL;

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum+1];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum+1];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = (int)(pCpuNet->MAX_DELAY/dt);
	printf("MAX_DELAY: %lf %lf %lf\n", network->maxDelay, pCpuNet->MAX_DELAY, dt);
	printf("MAX_DELAY: %u\n", MAX_DELAY);

	//GLIFNeurons *pN = (GLIFNeurons*)pCpuNet->pNeurons;
	//GExpSynapses *pS = (GExpSynapses*)pCpuNet->pSynapses;
	checkCudaErrors(cudaMalloc((void**)&c_gTimeTable, sizeof(int)*(MAX_DELAY+1)));
	checkCudaErrors(cudaMemset(c_gTimeTable, 0, sizeof(int)*(MAX_DELAY+1)));
	//TODO: need to adapt the length
	checkCudaErrors(cudaMalloc((void**)&c_gFiredTable, sizeof(bool)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMemset(c_gFiredTable, 0, sizeof(bool)*((totalNeuronNum)*(MAX_DELAY+1))));
	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesFiredTable, sizeof(bool)*(totalSynapseNum)));
	checkCudaErrors(cudaMemset(c_gSynapsesFiredTable, 0, sizeof(bool)*(totalSynapseNum)));

#ifdef TEST
	GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	checkCudaErrors(cudaMemcpy(pNTmp, ((GLIFNeurons*)(c_pGpuNet->pNeurons)), sizeof(GLIFNeurons), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pSTmp, ((GExpSynapses*)(c_pGpuNet->pSynapses)), sizeof(GExpSynapses), cudaMemcpyDeviceToHost));
	real *g_n_vm = pNTmp->p_vm;
	real *g_s_vm = pSTmp->p_I_syn;
	free(pNTmp);
	free(pSTmp);
#endif

	init_global<<<1, 1, 0>>>(MAX_DELAY, c_gTimeTable, MAX_DELAY+1, c_gFiredTable, pN->num, c_gSynapsesFiredTable, pS->num);

	printf("Start runing for %d cycles\n", sim_cycle);
	for (int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);
		//update_lif_neuron<<<3, 2, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, c_pGpuNet->neuronNum, time);
		for (int i=0; i<nTypeNum; i++) {
			updateType[c_pGpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], totalNeuronNum, simTime);
		}

		//update_pre_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_pre_synapse<<<1, 1, 0>>>(c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		//update_exp_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);
		for (int i=0; i<sTypeNum; i++) {
			updateType[c_pGpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], totalSynapseNum, simTime);
		}

		int currentIdx = time%(MAX_DELAY+1);
		checkCudaErrors(cudaMemcpy(c_n_fired, c_gFiredTable + (currentIdx*pN->num), sizeof(bool)*pN->num, cudaMemcpyDeviceToHost));

#ifdef TEST
		checkCudaErrors(cudaMemcpy(c_n_vm, g_n_vm, sizeof(real)*pCpuNet->neuronNum,cudaMemcpyDeviceToHost));
		checkCudaErrors(cudaMemcpy(c_s_vm, g_s_vm, sizeof(real)*pCpuNet->synapseNum,cudaMemcpyDeviceToHost));
		for (int i=0; i<pCpuNet->neuronNum; i++) {
			fprintf(fileN[i], "%lf\n", c_n_vm[i]); 
		}
		for (int i=0; i<pCpuNet->synapseNum; i++) {
			fprintf(fileS[i], "%lf\n", c_s_vm[i]); 
		}
#endif

		int count = 0;
		for (int i=0; i<pCpuNet->neuronNums[nTypeNum+1]; i++) {
			if (c_n_fired[i]) {
				count++;
			}
		}
		if (count > 0) {
			fprintf(logFile, "Cycle %d: ", time);
			for (int i=0; i<pCpuNet->neuronNum; i++) {
				if (c_n_fired[i]) {
					fprintf(logFile, " %d_%d", pN->pID[i].groupId, pN->pID[i].id);
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

#ifdef TEST
	for (int i=0; i<pCpuNet->neuronNum; i++) {
		fflush(fileN[i]);
		fclose(fileN[i]);
	}
	for (int i=0; i<pCpuNet->synapseNum; i++) {
		fflush(fileS[i]);
		fclose(fileS[i]);
	}

	free(fileN);
	free(fileS);
#endif

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
	checkCudaErrors(cudaMemcpy(nTypes, pCpuNet->nsypes, sizeof(Type)*sTypeNum, cudaMemcpyHostToDevice));

	int *neuronNums, *synapseNums;
	checkCudaErrors(cudaMalloc((void**)&(neuronNums), sizeof(int)*(nTypeNum+1)));
	checkCudaErrors(cudaMemcpy(neuronNums, pCpuNet->neuronNums, sizeof(int)*(nTypeNum+1), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(synapseNums), sizeof(int)*(sTypeNum+1)));
	checkCudaErrors(cudaMemcpy(synapseNums, pCpuNet->synapseNums, sizeof(int)*(sTypeNum+1), cudaMemcpyHostToDevice));

	//TODO support multitype N and S
	void **pNs = malloc(sizeof(void*)*nTypeNum);
	void **pSs = malloc(sizeof(void*)*sTypeNum);

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

	void **pTmpNs = malloc(sizeof(void*) * nTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpNs, pTmpNet->pNeurons, sizeof(void*)*nTypeNum, cudaMemcpyDeviceToHost));
	void **pTmpSs = malloc(sizeof(void*) * sTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpSs, pTmpNet->pSynapses, sizeof(void*)*sTypeNum, cudaMemcpyDeviceToHost));

	Type * pTmpNT = malloc(sizeof(Type)*nTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpNTs, pTmpNet->nTypes, sizeof(void*)*nTypeNum, cudaMemcpyDeviceToHost));
	Type * pTmpST = malloc(sizeof(Type)*sTypeNum);
	checkCudaErrors(cudaMemcpy(pTmpSTs, pTmpNet->sTypes, sizeof(void*)*sTypeNum, cudaMemcpyDeviceToHost));

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
