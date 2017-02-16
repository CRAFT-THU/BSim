/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */

#include "../third_party/cuda/helper_cuda.h"
#include "GExpSynapses.h"

int cudaAllocExp(void *pCpu, void *pGpu, int num)
{
	GExpSynapses *pGpuSynapses = (GExpSynapses*)pGpu;
	GExpSynapses *p = (GExpSynapses*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_weight), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_weight, p->p_weight, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_delay_steps), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_delay_steps, p->p_delay_steps, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_C1, p->p_C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__C1, p->p__C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_I_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_I_syn, p->p_I_syn, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pDst), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pDst, p->pDst, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeExp(void *pGpu)
{
	GExpSynapses *pGpuSynapses = (GExpSynapses*)pGpu;

	checkCudaErrors(cudaFree(pGpuSynapses->p_weight));
	checkCudaErrors(cudaFree(pGpuSynapses->p_delay_steps));
	checkCudaErrors(cudaFree(pGpuSynapses->p_C1));
	checkCudaErrors(cudaFree(pGpuSynapses->p__C1));
	checkCudaErrors(cudaFree(pGpuSynapses->p_I_syn));

	checkCudaErrors(cudaFree(pGpuSynapses->pDst));

	return 0;
}
