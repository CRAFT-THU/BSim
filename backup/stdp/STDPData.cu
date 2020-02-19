#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "STDPData.h"

void *cudaMallocSTDP()
{
	void *ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(STDPData)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(STDPData)*1));
	return ret;
}

void *cudaAllocSTDP(void *pCPU, int num)
{
	void *ret = cudaMallocSTDP();
	void *tmp = cudaAllocSTDPPara(pCPU, num);
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(STDPData)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}

void *cudaAllocSTDPPara(void *pCPU, int num)
{
	STDPData *p = (STDPData*)pCPU;
	STDPData *ret = (STDPData*)malloc(sizeof(STDPData)*1);
	memset(ret, 0, sizeof(STDPData)*1);

	checkCudaErrors(cudaMalloc((void**)&(ret->pDst), sizeof(int)*num));
	checkCudaErrors(cudaMemset(ret->pDst, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(ret->pDst, p->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pLastUpdate), sizeof(int)*num));
	checkCudaErrors(cudaMemset(ret->pLastUpdate, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(ret->pLastUpdate, p->pLastUpdate, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(ret->pWeight), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pWeight, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pWeight, p->pWeight, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pAPre), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pAPre, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pAPre, p->pAPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pAPost), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pAPost, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pAPost, p->pAPost, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pDPre), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pDPre, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pDPre, p->pDPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pDPost), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pDPost, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pDPost, p->pDPost, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pTauPre), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pTauPre, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pTauPre, p->pTauPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pTauPost), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pTauPost, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pTauPost, p->pTauPost, sizeof(real)*num, cudaMemcpyHostToDevice));

	return ret;
}

int cudaFetchSTDP(void *pCPU, void *pGPU, int num)
{
	STDPData *pTmp = (STDPData*)malloc(sizeof(STDPData)*1);
	memset(pTmp, 0, sizeof(STDPData)*1);
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(STDPData)*1, cudaMemcpyDeviceToHost));

	cudaSTDPParaFromGPU(pCPU, pTmp, num);
	return 0;
}

int cudaSTDPParaToGPU(void *pCPU, void *pGPU, int num)
{
	STDPData *pC = (STDPData*)pCPU;
	STDPData *pG = (STDPData*)pGPU;

	checkCudaErrors(cudaMemcpy(pG->pDst, pC->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pLastUpdate, pC->pLastUpdate, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy(pG->pWeight, pC->pWeight, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pAPre, pC->pAPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pAPost, pC->pAPost, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pDPre, pC->pDPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pDPost, pC->pDPost, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pTauPre, pC->pTauPre, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pTauPost, pC->pTauPost, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaSTDPParaFromGPU(void *pCPU, void *pGPU, int num)
{
	STDPData *pC = (STDPData*)pCPU;
	STDPData *pG = (STDPData*)pGPU;

	checkCudaErrors(cudaMemcpy(pC->pDst, pG->pDst, sizeof(int)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pLastUpdate, pG->pLastUpdate, sizeof(int)*num, cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaMemcpy(pC->pWeight, pG->pWeight, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pAPre, pG->pAPre, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pAPost, pG->pAPost, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pDPre, pG->pDPre, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pDPost, pG->pDPost, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pTauPre, pG->pTauPre, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pTauPost, pG->pTauPost, sizeof(real)*num, cudaMemcpyDeviceToHost));

	return 0;
}

int cudaFreeSTDP(void *pGPU)
{
	STDPData *tmp = (STDPData*)malloc(sizeof(STDPData)*1);
	memset(tmp, 0, sizeof(STDPData)*1);
	checkCudaErrors(cudaMemcpy(tmp, pGPU, sizeof(STDPData)*1, cudaMemcpyDeviceToHost));
	cudaFreeSTDPPara(tmp);
	free(tmp);
	tmp = NULL;
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}

int cudaFreeSTDPPara(void *pGPU)
{
	STDPData *p = (STDPData*)pGPU;
	cudaFree(p->pDst);
	p->pDst = NULL;
	cudaFree(p->pLastUpdate);
	p->pLastUpdate = NULL;

	cudaFree(p->pWeight);
	p->pWeight = NULL;
	cudaFree(p->pAPre);
	p->pAPre = NULL;
	cudaFree(p->pAPost);
	p->pAPost = NULL;
	cudaFree(p->pDPre);
	p->pDPre = NULL;
	cudaFree(p->pDPost);
	p->pDPost = NULL;
	cudaFree(p->pTauPre);
	p->pTauPre = NULL;
	cudaFree(p->pTauPost);
	p->pTauPost = NULL;

	return 0;
}

