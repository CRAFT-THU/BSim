#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "StaticData.h"

void *cudaMallocStatic()
{
	void *ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(StaticData)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(StaticData)*1));
	return ret;
}

void *cudaAllocStatic(void *pCPU, int num)
{
	void *ret = cudaMallocStatic();
	void *tmp = cudaAllocStaticPara(pCPU, num);
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(StaticData)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}

void *cudaAllocStaticPara(void *pCPU, int num)
{
	StaticData *p = (StaticData*)pCPU;
	StaticData *ret = (StaticData*)malloc(sizeof(StaticData)*1);
	memset(ret, 0, sizeof(StaticData)*1);

	checkCudaErrors(cudaMalloc((void**)&(ret->pDst), sizeof(int)*num));
	checkCudaErrors(cudaMemset(ret->pDst, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(ret->pDst, p->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(ret->pWeight), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pWeight, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pWeight, p->pWeight, sizeof(real)*num, cudaMemcpyHostToDevice));

	return ret;
}

int cudaFetchStatic(void *pCPU, void *pGPU, int num)
{
	StaticData *pTmp = (StaticData*)malloc(sizeof(StaticData)*1);
	memset(pTmp, 0, sizeof(StaticData)*1);
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(StaticData)*1, cudaMemcpyDeviceToHost));

	cudaStaticParaFromGPU(pCPU, pTmp, num);
	return 0;
}

int cudaStaticParaToGPU(void *pCPU, void *pGPU, int num)
{
	StaticData *pC = (StaticData*)pCPU;
	StaticData *pG = (StaticData*)pGPU;

	checkCudaErrors(cudaMemcpy(pG->pDst, pC->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy(pG->pWeight, pC->pWeight, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaStaticParaFromGPU(void *pCPU, void *pGPU, int num)
{
	StaticData *pC = (StaticData*)pCPU;
	StaticData *pG = (StaticData*)pGPU;

	checkCudaErrors(cudaMemcpy(pC->pDst, pG->pDst, sizeof(int)*num, cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMemcpy(pC->pWeight, pG->pWeight, sizeof(real)*num, cudaMemcpyDeviceToHost));

	return 0;
}

int cudaFreeStatic(void *pGPU)
{
	StaticData *tmp = (StaticData*)malloc(sizeof(StaticData)*1);
	memset(tmp, 0, sizeof(StaticData)*1);
	checkCudaErrors(cudaMemcpy(tmp, pGPU, sizeof(StaticData)*1, cudaMemcpyDeviceToHost));
	cudaFreeStaticPara(tmp);
	free(tmp);
	tmp = NULL;
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}

int cudaFreeStaticPara(void *pGPU)
{
	StaticData *p = (StaticData*)pGPU;
	cudaFree(p->pDst);
	p->pDst = NULL;

	cudaFree(p->pWeight);
	p->pWeight = NULL;

	return 0;
}

