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

	checkCudaErrors(cudaMalloc((void**)&(tmp->pI_i), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pI_i, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pI_i, p->pI_i, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_reset), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_reset, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_reset, p->pV_reset, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pCe), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pCe, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pCe, p->pCe, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pC_e), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pC_e, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pC_e, p->pC_e, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pC_m), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pC_m, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pC_m, p->pC_m, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pI_e), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pI_e, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pI_e, p->pI_e, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_tmp, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_tmp, p->pV_tmp, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pCi), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pCi, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pCi, p->pCi, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_thresh), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_thresh, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_thresh, p->pV_thresh, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_m), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_m, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_m, p->pV_m, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_i), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_i, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_i, p->pV_i, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pV_e), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pV_e, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pV_e, p->pV_e, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pC_i), sizeof(real)*num));
	checkCudaErrors(cudaMemset(tmp->pC_i, 0, sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(tmp->pC_i, p->pC_i, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(tmp->pRefracTime), sizeof(int)*num));
	checkCudaErrors(cudaMemset(tmp->pRefracTime, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(tmp->pRefracTime, p->pRefracTime, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(tmp->pRefracStep), sizeof(int)*num));
	checkCudaErrors(cudaMemset(tmp->pRefracStep, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(tmp->pRefracStep, p->pRefracStep, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(GLIFNeurons)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(GLIFNeurons)*1));
	checkCudaErrors(cudaMemcpy(ret, tmp, sizeof(GLIFNeurons)*1, cudaMemcpyHostToDevice));
	free(tmp);
	tmp = NULL;
	return ret;
}

int cudaLIFToGPU(void *pCPU, void *pGPU, int num)
{
	GLIFNeurons *pC = (GLIFNeurons*)pCPU;
	GLIFNeurons *pG = (GLIFNeurons*)pGPU;

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
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(pG, pC, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeLIF(void *pGPU)
{
	cudaFree(pGPU);
	pGPU = NULL;
	return 0;
}

