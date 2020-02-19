#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "TraubMilesData.h"

void *cudaMallocTraubMiles()
{
	void *ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(TraubMilesData)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(TraubMilesData)*1));
	return ret;
}


void *cudaAllocTraubMiles(void *pCPU, int num)
{
	void *ret = cudaMallocTraubMiles();
	void *tmp = cudaAllocTraubMilesPara(pCPU, num);
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(TraubMilesData)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}


void *cudaAllocTraubMilesPara(void *pCPU, int num)
{
	TraubMilesData *p = (TraubMilesData*)pCPU;
	TraubMilesData *ret = (TraubMilesData*)malloc(sizeof(TraubMilesData)*1);
	memset(ret, 0, sizeof(TraubMilesData)*1);

	checkCudaErrors(cudaMalloc((void**)&(ret->pGNa), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pGNa, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pGNa, p->pGNa, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pENa), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pENa, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pENa, p->pENa, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pGK), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pGK, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pGK, p->pGK, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pEK), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pEK, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pEK, p->pEK, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pGl), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pGl, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pGl, p->pGl, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pEl), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pEl, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pEl, p->pEl, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pC), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pC, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pC, p->pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pV), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pV, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pV, p->pV, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pM), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pM, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pM, p->pM, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pH), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pH, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pH, p->pH, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pN), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pN, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pN, p->pN, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pDecayE), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pDecayE, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pDecayE, p->pDecayE, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pDecayI), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pDecayI, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pDecayI, p->pDecayI, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pEE), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pEE, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pEE, p->pEE, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(ret->pEI), sizeof(real)*num));
	checkCudaErrors(cudaMemset(ret->pEI, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(ret->pEI, p->pEI, sizeof(real)*num, cudaMemcpyHostToDevice));


	return ret;
}


int cudaFetchTraubMiles(void *pCPU, void *pGPU, int num)
{
	TraubMilesData *pTmp = (TraubMilesData*)malloc(sizeof(TraubMilesData)*1);
	memset(pTmp, 0, sizeof(TraubMilesData)*1);
	checkCudaErrors(cudaMemcpy(pTmp, pGPU, sizeof(TraubMilesData)*1, cudaMemcpyDeviceToHost));

	cudaTraubMilesParaFromGPU(pCPU, pTmp, num);
	return 0;
}


int cudaTraubMilesParaToGPU(void *pCPU, void *pGPU, int num)
{
	TraubMilesData *pC = (TraubMilesData*)pCPU;
	TraubMilesData *pG = (TraubMilesData*)pGPU;

	checkCudaErrors(cudaMemcpy(pG->pGNa, pC->pGNa, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pENa, pC->pENa, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pGK, pC->pGK, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pEK, pC->pEK, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pGl, pC->pGl, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pEl, pC->pEl, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pC, pC->pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pV, pC->pV, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pM, pC->pM, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pH, pC->pH, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pN, pC->pN, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pDecayE, pC->pDecayE, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pDecayI, pC->pDecayI, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pEE, pC->pEE, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG->pEI, pC->pEI, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}


int cudaTraubMilesParaFromGPU(void *pCPU, void *pGPU, int num)
{
	TraubMilesData *pC = (TraubMilesData*)pCPU;
	TraubMilesData *pG = (TraubMilesData*)pGPU;

	checkCudaErrors(cudaMemcpy(pC->pGNa, pG->pGNa, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pENa, pG->pENa, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pGK, pG->pGK, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pEK, pG->pEK, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pGl, pG->pGl, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pEl, pG->pEl, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pC, pG->pC, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pV, pG->pV, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pM, pG->pM, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pH, pG->pH, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pN, pG->pN, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pDecayE, pG->pDecayE, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pDecayI, pG->pDecayI, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pEE, pG->pEE, sizeof(real)*num, cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pC->pEI, pG->pEI, sizeof(real)*num, cudaMemcpyDeviceToHost));

	return 0;
}


int cudaFreeTraubMiles(void *pGPU)
{
	TraubMilesData *tmp = (TraubMilesData*)malloc(sizeof(TraubMilesData)*1);
	memset(tmp, 0, sizeof(TraubMilesData)*1);
	checkCudaErrors(cudaMemcpy(tmp, pGPU, sizeof(TraubMilesData)*1, cudaMemcpyDeviceToHost));
	cudaFreeTraubMilesPara(tmp);
	free(tmp);
	tmp = NULL;
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}


int cudaFreeTraubMilesPara(void *pGPU)
{
	TraubMilesData *p = (TraubMilesData*)pGPU;
	cudaFree(p->pGNa);
	p->pGNa = NULL;
	cudaFree(p->pENa);
	p->pENa = NULL;
	cudaFree(p->pGK);
	p->pGK = NULL;
	cudaFree(p->pEK);
	p->pEK = NULL;
	cudaFree(p->pGl);
	p->pGl = NULL;
	cudaFree(p->pEl);
	p->pEl = NULL;
	cudaFree(p->pC);
	p->pC = NULL;
	cudaFree(p->pV);
	p->pV = NULL;
	cudaFree(p->pM);
	p->pM = NULL;
	cudaFree(p->pH);
	p->pH = NULL;
	cudaFree(p->pN);
	p->pN = NULL;
	cudaFree(p->pDecayE);
	p->pDecayE = NULL;
	cudaFree(p->pDecayI);
	p->pDecayI = NULL;
	cudaFree(p->pEE);
	p->pEE = NULL;
	cudaFree(p->pEI);
	p->pEI = NULL;

	return 0;
}


