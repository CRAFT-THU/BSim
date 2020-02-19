
#include <stdlib.h>
#include <assert.h>

#include "../gpu_utils/mem_op.h"
#include "Connection.h"

Connection * cudaAllocConnection(Connection * pCPU)
{
	int nNum = pCPU->nNum;
	int sNum = pCPU->sNum;
	int length = (pCPU->maxDelay - pCPU->minDelay + 1) * nNum;

	Connection * pGPU = NULL;
	Connection *pTmp = (Connection*)malloc(sizeof(Connection));
	pTmp->nNum = nNum;
	pTmp->sNum = sNum;
	pTmp->maxDelay = pCPU->maxDelay;
	pTmp->minDelay = pCPU->minDelay;

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pDelayStart), sizeof(int)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pDelayStart, pCPU->pDelayStart, sizeof(int)*length, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pDelayNum), sizeof(int)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pDelayNum, pCPU->pDelayNum, sizeof(int)*length, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pDelayStartRev), sizeof(int)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pDelayStartRev, pCPU->pDelayStartRev, sizeof(int)*length, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pDelayNumRev), sizeof(int)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pDelayNumRev, pCPU->pDelayNumRev, sizeof(int)*length, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pSidMapRev), sizeof(int)*sNum));
	checkCudaErrors(cudaMemcpy(pTmp->pSidMapRev, pCPU->pSidMapRev, sizeof(int)*sNum, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGPU), sizeof(Connection)));
	checkCudaErrors(cudaMemcpy(pGPU, pTmp, sizeof(Connection), cudaMemcpyHostToDevice));

	free(pTmp);
	pTmp = NULL;

	return pGPU;
}

int cudaFetchConnection(Connection *pCPU, Connection *pGPU)
{
	int nNum = pCPU->nNum;
	int sNum = pCPU->sNum;
	int length = (pCPU->maxDelay - pCPU->minDelay + 1) * nNum;

	Connection *pTmp = (Connection*)malloc(sizeof(Connection));
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(Connection), cudaMemcpyDeviceToHost));
	assert(nNum == pTmp->nNum);
	assert(sNum == pTmp->sNum);
	assert(pCPU->maxDelay == pTmp->maxDelay);
	assert(pCPU->minDelay == pTmp->minDelay);

	checkCudaErrors(cudaMemcpy(pCPU->pDelayStart, pTmp->pDelayStart, sizeof(int)*length, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pCPU->pDelayNum, pTmp->pDelayNum, sizeof(int)*length, cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMemcpy(pCPU->pDelayStartRev, pTmp->pDelayStartRev, sizeof(int)*length, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pCPU->pDelayNumRev, pTmp->pDelayNumRev, sizeof(int)*length, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pCPU->pSidMapRev, pTmp->pSidMapRev, sizeof(int)*sNum, cudaMemcpyDeviceToHost));

	free(pTmp);
	pTmp = NULL;

	return 0;
}

int cudaFreeConnection(Connection *pGPU)
{
	Connection * pTmp = (Connection*)malloc(sizeof(Connection));
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(Connection), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaFree(pTmp->pDelayStart));
	checkCudaErrors(cudaFree(pTmp->pDelayNum));
	checkCudaErrors(cudaFree(pTmp->pDelayStartRev));
	checkCudaErrors(cudaFree(pTmp->pDelayNumRev));
	checkCudaErrors(cudaFree(pTmp->pSidMapRev));
	free(pTmp);
	pTmp = NULL;
	return 0;
}

LolConnection * cudaAllocLolConnection(LolConnection * pCPU)
{
	int nNum = pCPU->nNum;
	int sNum = pCPU->sNum;
	int length = (pCPU->maxDelay - pCPU->minDelay + 1) * nNum;

	LolConnection * pGPU = NULL;
	LolConnection *pTmp = (LolConnection*)malloc(sizeof(LolConnection));
	pTmp->nNum = nNum;
	pTmp->sNum = sNum;
	pTmp->maxDelay = pCPU->maxDelay;
	pTmp->minDelay = pCPU->minDelay;

	int **pIdx = (int**)malloc(sizeof(int*)*length); 
	for (int i=0; i<length; i++) {
		if (pCPU->pNum[i] > 0) {
			checkCudaErrors(cudaMalloc((void**)&(pIdx[i]), sizeof(int)*(pCPU->pNum[i])));
			checkCudaErrors(cudaMemcpy(pIdx[i], pCPU->pIndex[i], sizeof(int)*(pCPU->pNum[i]), cudaMemcpyHostToDevice));
		} else {
			pIdx[i] = NULL;
		}
	}

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pIndex), sizeof(int*)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pIndex, pIdx, sizeof(int*)*length, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->pNum), sizeof(int)*length));
	checkCudaErrors(cudaMemcpy(pTmp->pNum, pCPU->pNum, sizeof(int)*length, cudaMemcpyHostToDevice));


	checkCudaErrors(cudaMalloc((void**)&(pGPU), sizeof(LolConnection)));
	checkCudaErrors(cudaMemcpy(pGPU, pTmp, sizeof(LolConnection), cudaMemcpyHostToDevice));

	free(pTmp);
	pTmp = NULL;
	free(pIdx);
	pIdx = NULL;

	return pGPU;
}

// int cudaFetchLolConnection(LolConnection *pCPU, LolConnection *pGPU)
// {
// 	int nNum = pCPU->nNum;
// 	int sNum = pCPU->sNum;
// 	int length = (pCPU->maxDelay - pCPU->minDelay + 1) * nNum;
// 
// 	Connection *pTmp = (Connection*)malloc(sizeof(Connection));
// 	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(Connection), cudaMemcpyDeviceToHost));
// 	assert(nNum == pTmp->nNum);
// 	assert(sNum == pTmp->sNum);
// 	assert(pCPU->maxDelay == pTmp->maxDelay);
// 	assert(pCPU->minDelay == pTmp->minDelay);
// 
// 	checkCudaErrors(cudaMemcpy(pCPU->pDelayStart, pTmp->pDelayStart, sizeof(int)*length, cudaMemcpyDeviceToHost));
// 	checkCudaErrors(cudaMemcpy(pCPU->pDelayNum, pTmp->pDelayNum, sizeof(int)*length, cudaMemcpyDeviceToHost));
// 
// 	free(pTmp);
// 	pTmp = NULL;
// 
// 	return 0;
// }

int cudaFreeLolConnection(LolConnection *pGPU)
{
	LolConnection * pTmp = (LolConnection*)malloc(sizeof(LolConnection));
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(LolConnection), cudaMemcpyDeviceToHost));
	int length = (pTmp->maxDelay - pTmp->minDelay + 1) * pTmp->nNum;

	int *num = (int*)malloc(sizeof(int)*length);
	int **index = (int**)malloc(sizeof(int*)*length);
	checkCudaErrors(cudaMemcpy(num, pTmp->pNum, sizeof(int)*length, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(index, pTmp->pIndex, sizeof(int*)*length, cudaMemcpyDeviceToHost));
	for (int i=0; i<length; i++) {
		if (num[i] > 0) {
			checkCudaErrors(cudaFree(index[i]));
		}
	}
	checkCudaErrors(cudaFree(pTmp->pIndex));
	checkCudaErrors(cudaFree(pTmp->pNum));
	free(pTmp);
	pTmp = NULL;
	free(index);
	index = NULL;
	free(num);
	num = NULL;
	// checkCudaErrors(cudaFree(pGPU));
	return 0;
}
