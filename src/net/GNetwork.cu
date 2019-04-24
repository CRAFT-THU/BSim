#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "../neuron/lif/GLIFNeurons.h"
#include "../utils/TypeFunc.h"
#include "../utils/macros.h"
#include "../gpu_utils/mem_op.h"
#include "GNetwork.h"

GNetwork* copyNetworkToGPU(GNetwork *pCpuNet)
{
	if (pCpuNet == NULL) {
		printf("NULL POINTER: GNETWORK\n");
		return NULL;
	}

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;

	GNetwork *tmp = allocNetwork(nTypeNum, sTypeNum);

	// tmp->maxDelay = pCpuNet->maxDelay;
	// tmp->minDelay = pCpuNet->minDelay;

	for (int i=0; i<nTypeNum; i++) {
		tmp->pNTypes[i] = pCpuNet->pNTypes[i];
		tmp->pNeuronNums[i] = pCpuNet->pNeuronNums[i];
	}
	tmp->pNeuronNums[nTypeNum] = pCpuNet->pNeuronNums[nTypeNum];

	for (int i=0; i<sTypeNum; i++) {
		tmp->pSTypes[i] = pCpuNet->pSTypes[i];
		tmp->pSynapseNums[i] = pCpuNet->pSynapseNums[i];
	}
	tmp->pSynapseNums[sTypeNum] = pCpuNet->pSynapseNums[sTypeNum];



	for (int i=0; i<nTypeNum; i++) {
		tmp->ppNeurons[i] = cudaAllocType[pCpuNet->pNTypes[i]](pCpuNet->ppNeurons[i], pCpuNet->pNeuronNums[i+1]-pCpuNet->pNeuronNums[i]);
	}

	for (int i=0; i<sTypeNum; i++) {
		tmp->ppSynapses[i] = cudaAllocType[pCpuNet->pSTypes[i]](pCpuNet->ppSynapses[i], pCpuNet->pSynapseNums[i+1]-pCpuNet->pSynapseNums[i]);
	}

	tmp->pConnection = copyConnectionToGPU(pCpuNet->pConnection);

	return tmp;
}

int fetchNetworkFromGPU(GNetwork *pCpuNet, GNetwork *pGpuNet)
{
	if (pCpuNet == NULL && pGpuNet == NULL) {
		printf("NULL POINTER: GNETWORK\n");
		return -1;
	}

	int nTypeNum = pCpuNet->nTypeNum;

	//TODO support multitype N and S
	for (int i=0; i<nTypeNum; i++) {
		//TODO: cudaFetchType
		//cudaFetchType[pCpuNet->nTypes[i]](pGpuNet->pNeurons[i], pCpuNet->pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i]);
	}

	return 0;
}

int freeNetworkGPU(GNetwork *pGpuNet)
{
	GNetwork *pTmp = pGpuNet;

	int nTypeNum = pTmp->nTypeNum;
	int sTypeNum = pTmp->sTypeNum;

	for (int i=0; i<nTypeNum; i++) {
		void *pTmpN = mallocType[pTmp->pNTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpN, pTmp->ppNeurons[i], getTypeSize[pTmp->pNTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmp->nTypes[i]](pTmpN);
		checkCudaErrors(cudaFree(pTmp->pNeurons[i]));
		free(pTmpN);
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pTmpS = mallocType[pTmp->sTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpS, pTmp->pSynapses[i], getTypeSize[pTmp->sTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmp->sTypes[i]](pTmpS);
		checkCudaErrors(cudaFree(pTmp->pSynapses[i]));
		free(pTmpS);
	}

	N2SConnection * pConnection = (N2SConnection*)malloc(sizeof(N2SConnection));
	checkCudaErrors(cudaMemcpy(pConnection, pTmp->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));
	//checkCudaErrors(cudaFree(pConnection->pSynapsesIdx));
	checkCudaErrors(cudaFree(pConnection->delayStart));
	checkCudaErrors(cudaFree(pConnection->delayNum));
	checkCudaErrors(cudaFree(pTmp->pN2SConnection));
	free(pConnection);

	free(pTmp->ppNeurons);
	free(pTmp->ppSynapses);
	free(pTmp);

	return 0;
}



int checkGPUNetwork(GNetwork *g, GNetwork *c)
{
	int ret = -1;

	CHECK(g, c, nTypeNum);
	CHECK(g, c, sTypeNum);
	CHECK(g, c, neuronNums);
	CHECK(g, c, synapseNums);

	CHECK(g, c, nTypes);
	CHECK(g, c, sTypes);

	ret = 1;

	//int totalNeuronNum = g->neuronNums[g->nTypeNum+1];
	//int totalSynapseNum = g->synapseNums[g->sTypeNum+1];
	int maxDelay = c->maxDelay;
	int minDelay = c->minDelay;

	N2SConnection p;
	checkCudaErrors(cudaMemcpy(&p, g->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));

	//CHECK_CROSS_ARRAY(p.pSynapsesIdx, c->pN2SConnection->pSynapsesIdx, sizeof(int)*(c->pN2SConnection->s_num));
	CHECK_GPU_TO_CPU_ARRAY(p.delayStart, c->pN2SConnection->delayStart, sizeof(int)*(c->pN2SConnection->nNum)*(maxDelay-minDelay+1);
	CHECK_GPU_TO_CPU_ARRAY(p.delayNum, c->pN2SConnection->delayNum, sizeof(int)*(c->pN2SConnection->nNum)*(maxDelay-minDelay+1));

	ret = 2;

	return ret;
}
