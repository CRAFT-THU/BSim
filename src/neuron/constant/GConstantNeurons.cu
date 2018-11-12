/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../../third_party/cuda/helper_cuda.h"
#include "GConstantNeurons.h"

int cudaAllocConstant(void *pCpu, void *pGpu, int num)
{
	GConstantNeurons *pGpuNeurons = (GConstantNeurons*)pGpu;
	GConstantNeurons *p = (GConstantNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_fire_rate), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_fire_rate, p->p_fire_rate, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_start_cycle), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_start_cycle, p->p_start_cycle, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_end_cycle), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_end_cycle, p->p_end_cycle, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_fire_count), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_fire_count, p->p_fire_count, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeConstant(void *pGpu)
{
	GConstantNeurons *pGpuNeurons = (GConstantNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->p_fire_rate));
	checkCudaErrors(cudaFree(pGpuNeurons->p_start_cycle));
	checkCudaErrors(cudaFree(pGpuNeurons->p_end_cycle));
	checkCudaErrors(cudaFree(pGpuNeurons->p_fire_count));

	return 0;
}
