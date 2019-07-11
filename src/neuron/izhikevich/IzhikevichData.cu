#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "IzhikevichData.h"

void *cudaMallocIzhikevich()
{
	void *ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(IzhikevichData)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(IzhikevichData)*1));
	return ret;
}

void *cudaAllocIzhikevich(void *pCPU, int num)
{
	void *ret = cudaMallocIzhikevich();
	void *tmp = cudaAllocIzhikevichPara(pCPU, num);
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(IzhikevichData)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}

void *cudaAllocIzhikevichPara(void *pCPU, int num)
{
	IzhikevichData *p = (IzhikevichData*)pCPU;
	IzhikevichData *ret = (IzhikevichData*)malloc(sizeof(IzhikevichData)*1);
	memset(ret, 0, sizeof(IzhikevichData)*1);

	checkCudaErrors(cudaMalloc((void**)&(ret->pV), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pV, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pV, p->pV, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pU), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pU, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pU, p->pU, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pA), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pA, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pA, p->pA, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pB), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pB, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pB, p->pB, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pC), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pC, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pC, p->pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pD), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pD, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pD, p->pD, sizeof(real)*num, cudaMemcpyHostToDevice));

	return ret;
}

int cudaFetchIzhikevich(void *pCPU, void *pGPU, int num)
{
	IzhikevichData *pTmp = (IzhikevichData*)malloc(sizeof(IzhikevichData)*1);
	memset(pTmp, 0, sizeof(IzhikevichData)*1);
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(IzhikevichData)*1, cudaMemcpyDeviceToHost));

	cudaIzhikevichParaFromGPU(pCPU, pTmp, num);
	return 0;
}

int cudaIzhikevichParaToGPU(void *pCPU, void *pGPU, int num)
{
	IzhikevichData *pC = (IzhikevichData*)pCPU;
	IzhikevichData *pG = (IzhikevichData*)pGPU;

	checkCudaErrors(cudaMemcpy(pG->pV, pC->pV, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pU, pC->pU, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pA, pC->pA, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pB, pC->pB, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pC, pC->pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pD, pC->pD, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaIzhikevichParaFromGPU(void *pCPU, void *pGPU, int num)
{
	IzhikevichData *pC = (IzhikevichData*)pCPU;
	IzhikevichData *pG = (IzhikevichData*)pGPU;

	checkCudaErrors(cudaMemcpy(pC->pV, pG->pV, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pU, pG->pU, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pA, pG->pA, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pB, pG->pB, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pC, pG->pC, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pD, pG->pD, sizeof(real)*num, cudaMemcpyDeviceToHost));

	return 0;
}

int cudaFreeIzhikevich(void *pGPU)
{
	IzhikevichData *tmp = (IzhikevichData*)malloc(sizeof(IzhikevichData)*1);
	memset(tmp, 0, sizeof(IzhikevichData)*1);
	checkCudaErrors(cudaMemcpy(tmp, pGPU, sizeof(IzhikevichData)*1, cudaMemcpyDeviceToHost));
	cudaFreeIzhikevichPara(tmp);
	free(tmp);
	tmp = NULL;
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}

int cudaFreeIzhikevichPara(void *pGPU)
{
	IzhikevichData *p = (IzhikevichData*)pGPU;
	cudaFree(p->pV);
	p->pV = NULL;
	cudaFree(p->pU);
	p->pU = NULL;
	cudaFree(p->pA);
	p->pA = NULL;
	cudaFree(p->pB);
	p->pB = NULL;
	cudaFree(p->pC);
	p->pC = NULL;
	cudaFree(p->pD);
	p->pD = NULL;

	return 0;
}

