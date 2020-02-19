#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "../neuron/lif/LIFData.h"
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
	tmp->pLolConnection = cudaAllocLolConnection(pCpuNet->pLolConnection);

	return tmp;
}

int fetchNetworkFromGPU(GNetwork *pCpuNet, GNetwork *pGpuNet)
{
	if (pCpuNet == NULL || pGpuNet == NULL) {
		printf("NULL POINTER: FETCH GNETWORK\n");
		return -1;
	}

	int nTypeNum = pGpuNet->nTypeNum;
	int sTypeNum = pGpuNet->sTypeNum;

	assert(pCpuNet->nTypeNum == nTypeNum);
	assert(pCpuNet->sTypeNum == sTypeNum);

	for (int i=0; i<nTypeNum; i++) {
		pCpuNet->pNTypes[i] = pGpuNet->pNTypes[i];
		pCpuNet->pNeuronNums[i] = pGpuNet->pNeuronNums[i];
	}
	pCpuNet->pNeuronNums[nTypeNum] = pGpuNet->pNeuronNums[nTypeNum];

	for (int i=0; i<sTypeNum; i++) {
		pCpuNet->pSTypes[i] = pGpuNet->pSTypes[i];
		pCpuNet->pSynapseNums[i] = pGpuNet->pSynapseNums[i];
	}
	pCpuNet->pSynapseNums[sTypeNum] = pGpuNet->pSynapseNums[sTypeNum];

	//TODO support multitype N and S
	for (int i=0; i<nTypeNum; i++) {
		//TODO: cudaFetchType
		cudaFetchType[pCpuNet->pNTypes[i]](pCpuNet->ppNeurons[i], pGpuNet->ppNeurons[i], pCpuNet->pNeuronNums[i+1]-pCpuNet->pNeuronNums[i]);
	}
	for (int i=0; i<sTypeNum; i++) {
		//TODO: cudaFetchType
		cudaFetchType[pCpuNet->pSTypes[i]](pCpuNet->ppSynapses[i], pGpuNet->ppSynapses[i], pCpuNet->pSynapseNums[i+1]-pCpuNet->pSynapseNums[i]);
	}

	cudaFetchConnection(pCpuNet->pConnection, pGpuNet->pConnection);
	return 0;
}

int freeNetworkGPU(GNetwork *pGpuNet)
{
	GNetwork *pTmp = pGpuNet;

	int nTypeNum = pTmp->nTypeNum;
	int sTypeNum = pTmp->sTypeNum;

	for (int i=0; i<nTypeNum; i++) {
		cudaFreeType[pTmp->pNTypes[i]](pTmp->ppNeurons[i]);
	}

	for (int i=0; i<sTypeNum; i++) {
		cudaFreeType[pTmp->pSTypes[i]](pTmp->ppSynapses[i]);
	}

	cudaFreeConnection(pTmp->pConnection);
	cudaFreeLolConnection(pTmp->pLolConnection);

	free(pTmp->ppNeurons);
	free(pTmp->ppSynapses);
	free(pTmp);

	return 0;
}



int checkNetworkGPU(GNetwork *g, GNetwork *c)
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
