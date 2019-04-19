#include <stdlib.h>
#include <string.h>
#include "../../third_party/cuda/helper_cuda.h"
#include "GLIFNeurons.h"

void *cudaAllocLIF(void *pCPU, int num)
{
	void *ret = NULL;
	GLIFNeurons *p = (GLIFNeurons*)pCPU;
	GLIFNeurons * tmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons)*1);
	memset(tmp, 0, sizeof(GLIFNeurons)*1);

	checkCudaErrors(cudaMalloc((void**)&(tmp->pRefracStep), sizeof(int)*num));
	checkCudaErrors(cudaMemset(tmp->pRefracStep, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(tmp->pRefracStep, p->pRefracStep, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pRefracTime), sizeof(int)*num));
	checkCudaErrors(cudaMemset(tmp->pRefracTime, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(tmp->pRefracTime, p->pRefracTime, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(tmp->pVm), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pVm, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pVm, p->pVm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pCi), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pCi, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pCi, p->pCi, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pCe), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pCe, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pCe, p->pCe, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pC_i), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pC_i, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pC_i, p->pC_i, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pC_e), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pC_e, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pC_e, p->pC_e, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pVtmp), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pVtmp, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pVtmp, p->pVtmp, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pIi), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pIi, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pIi, p->pIi, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pIe), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pIe, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pIe, p->pIe, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pVthresh), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pVthresh, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pVthresh, p->pVthresh, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pVreset), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pVreset, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pVreset, p->pVreset, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pCm), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pCm, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pCm, p->pCm, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(GLIFNeurons)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(GLIFNeurons)*1));
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(GLIFNeurons)*1, cudaMemcpyHostToDevice));
	free(tmp);
	{} = NULL;
	return ret;
}

void *cudaLIFToGPU(void *pCPU, void *pGPU, int num)
{
	GLIFNeurons *pC = (GLIFNeurons*)pCPU;
	LIFData *pG = (LIFData*)pGPU;

	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));

	return 0;
}

void *cudaFreeLIF(void *pGPU)
{
	cudaFree(pGPU);
	{} = NULL;
	return 0;
}

