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

	tmp->pConnection = cudaAllocConnection(pCpuNet->pConnection);

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
		//cudaFetchType[pCpuNet->pNTypes[i]](pGpuNet->ppNeurons[i], pCpuNet->ppNeurons[i], pCpuNet->pNeuronNums[i+1]-pCpuNet->pNeuronNums[i]);
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
		cudaFreeType[pTmp->pNTypes[i]](pTmpN);
		checkCudaErrors(cudaFree(pTmp->ppNeurons[i]));
		free(pTmpN);
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pTmpS = mallocType[pTmp->pSTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpS, pTmp->ppSynapses[i], getTypeSize[pTmp->pSTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmp->pSTypes[i]](pTmpS);
		checkCudaErrors(cudaFree(pTmp->ppSynapses[i]));
		free(pTmpS);
	}

	Connection * pConnection = (Connection*)malloc(sizeof(Connection));
	checkCudaErrors(cudaMemcpy(pConnection, pTmp->pConnection, sizeof(Connection), cudaMemcpyDeviceToHost));
	//checkCudaErrors(cudaFree(pConnection->ppSynapsesIdx));
	checkCudaErrors(cudaFree(pConnection->pDelayStart));
	checkCudaErrors(cudaFree(pConnection->pDelayNum));
	checkCudaErrors(cudaFree(pTmp->pConnection));
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
	CHECK(g, c, pNeuronNums);
	CHECK(g, c, pSynapseNums);

	CHECK(g, c, pNTypes);
	CHECK(g, c, pSTypes);

	ret = 1;

	//int totalNeuronNum = g->pNeuronNums[g->nTypeNum+1];
	//int totalSynapseNum = g->pSynapseNums[g->sTypeNum+1];
	// Connection p;
	// checkCudaErrors(cudaMemcpy(&p, g->pConnection, sizeof(Connection), cudaMemcpyDeviceToHost));

	// CHECK_GPU_TO_CPU_ARRAY(p.delayStart, c->pConnection->delayStart, sizeof(int)*(c->pConnection->nNum)*(maxDelay-minDelay+1));
	// CHECK_GPU_TO_CPU_ARRAY(p.delayNum, c->pConnection->delayNum, sizeof(int)*(c->pConnection->nNum)*(maxDelay-minDelay+1));

	ret = 2;

	return ret;
}
