/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../../third_party/cuda/helper_cuda.h"
#include "GFFTNeurons.h"

int cudaAllocFFT(void *pCpu, void *pGpu, int num)
{
	GFFTNeurons *pGpuNeurons = (GFFTNeurons*)pGpu;
	GFFTNeurons *p = (GFFTNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_input), sizeof(cufftComplex)*num/2));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_input, p->p_input, sizeof(cufftComplex)*num/2, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_res), sizeof(cufftComplex)*num/2));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_res, p->p_res, sizeof(cufftComplex)*num/2, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_fire_count), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_fire_count, p->p_fire_count, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeFFT(void *pGpu)
{
	GFFTNeurons *pGpuNeurons = (GFFTNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->p_input));
	checkCudaErrors(cudaFree(pGpuNeurons->p_res));
	checkCudaErrors(cudaFree(pGpuNeurons->p_fire_count));

	return 0;
}
