#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "GStaticSynapses.h"

void *cudaAllocStatic(void *pCPU, int num)
{
	void *ret = NULL;
	GStaticSynapses *p = (GStaticSynapses*)pCPU;
	GStaticSynapses *tmp = (GStaticSynapses*)malloc(sizeof(GStaticSynapses)*1);
	memset(tmp, 0, sizeof(GStaticSynapses)*1);

	checkCudaErrors(cudaMalloc((void**)&(tmp->pDst), sizeof(int)*num));
	checkCudaErrors(cudaMemset(tmp->pDst, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(tmp->pDst, p->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(tmp->pWeight), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pWeight, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pWeight, p->pWeight, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(GStaticSynapses)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(GStaticSynapses)*1));
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(GStaticSynapses)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}

int cudaStaticToGPU(void *pCPU, void *pGPU, int num)
{
	GStaticSynapses *pC = (GStaticSynapses*)pCPU;
	GStaticSynapses *pG = (GStaticSynapses*)pGPU;

	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeStatic(void *pGPU)
{
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}

