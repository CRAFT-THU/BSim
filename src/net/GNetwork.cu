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

	GNetwork *tmpNet = (GNetwork*)malloc(sizeof(GNetwork));
	memcpy(tmpNet, pCpuNet, sizeof(GNetwork));

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;

	//TODO support multitype N and S
	void **pNs = (void**)malloc(sizeof(void*)*nTypeNum);
	void **pSs = (void**)malloc(sizeof(void*)*sTypeNum);

	for (int i=0; i<nTypeNum; i++) {
		pNs[i] = cudaAllocType[pCpuNet->nTypes[i]](pCpuNet->pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i]);
	}

	for (int i=0; i<sTypeNum; i++) {
		pSs[i] = cudaAllocType[pCpuNet->sTypes[i]](pCpuNet->pSynapses[i], pCpuNet->synapseNums[i+1]-pCpuNet->synapseNums[i]);
	}

	tmpNet->pNeurons = pNs;
	tmpNet->pSynapses = pSs;
	tmpNet->pN2SConnection = copyConnectionToGPU(pCpuNet->pN2SConnection);
	tmpNet->neuronNums = pCpuNet->neuronNums;
	tmpNet->synapseNums = pCpuNet->synapseNums;

	tmpNet->nTypeNum = pCpuNet->nTypeNum;
	tmpNet->sTypeNum = pCpuNet->sTypeNum;

	tmpNet->nTypes = pCpuNet->nTypes;
	tmpNet->sTypes = pCpuNet->sTypes;

	tmpNet->maxDelay = pCpuNet->maxDelay;
	tmpNet->minDelay = pCpuNet->minDelay;

	return tmpNet;
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
		if (pCpuNet->nTypes[i] == LIF) {
			//cudaFetchLIF(pGpuNet->pNeurons[i], pCpuNet->pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i]);
		}
		//TODO: cudaFetchType
		//cudaFetchType[pCpuNet->nTypes[i]](pGpuNet->pNeurons[i], pCpuNet->pNeurons[i], pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i]);
	}

	return 0;
}

int freeNetworkGPU(GNetwork *pGpuNet)
{
	GNetwork *pTmpNet = pGpuNet;

	int nTypeNum = pTmpNet->nTypeNum;
	int sTypeNum = pTmpNet->sTypeNum;

	for (int i=0; i<nTypeNum; i++) {
		void *pTmpN = mallocType[pTmpNet->nTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpN, pTmpNet->pNeurons[i], getTypeSize[pTmpNet->nTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpNet->nTypes[i]](pTmpN);
		checkCudaErrors(cudaFree(pTmpNet->pNeurons[i]));
		free(pTmpN);
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pTmpS = mallocType[pTmpNet->sTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpS, pTmpNet->pSynapses[i], getTypeSize[pTmpNet->sTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpNet->sTypes[i]](pTmpS);
		checkCudaErrors(cudaFree(pTmpNet->pSynapses[i]));
		free(pTmpS);
	}

	N2SConnection * pConnection = (N2SConnection*)malloc(sizeof(N2SConnection));
	checkCudaErrors(cudaMemcpy(pConnection, pTmpNet->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));
	//checkCudaErrors(cudaFree(pConnection->pSynapsesIdx));
	checkCudaErrors(cudaFree(pConnection->delayStart));
	checkCudaErrors(cudaFree(pConnection->delayNum));
	checkCudaErrors(cudaFree(pTmpNet->pN2SConnection));
	free(pConnection);

	//checkCudaErrors(cudaFree(pTmpNet->nTypes));
	//checkCudaErrors(cudaFree(pTmpNet->sTypes));
	//free(pTmpNT);
	//free(pTmpST);

	//checkCudaErrors(cudaFree(pTmpNet->gNeuronNums));
	//checkCudaErrors(cudaFree(pTmpNet->gSynapseNums));

	//checkCudaErrors(cudaFree(pTmpNet->neuronNums));
	//checkCudaErrors(cudaFree(pTmpNet->synapseNums));

	//checkCudaErrors(cudaFree(pTmpNet->nOffsets));
	//checkCudaErrors(cudaFree(pTmpNet->sOffsets));

	free(pTmpNet->pNeurons);
	free(pTmpNet->pSynapses);
	//free(pTmpNs);
	//free(pTmpSs);
	free(pTmpNet);

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
