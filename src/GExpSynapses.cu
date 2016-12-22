/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */

#include "utils/cuda/helper_cuda.h"
#include "GExpSynapses.h"

int cudaAllocExpSynapses(void *pCpu, void *pGpu)
{
	GExpSynapses *pGpuSynapses = (GExpSynapses*)pGpu;
	GExpSynapses *p = (GExpSynapses*)pCpu;

	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pID), sizeof(ID)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->pID, p->pID, sizeof(ID)*p->num, cudaMemcpyHostToDevice));

	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pType), sizeof(Type)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->pType, p->pType, sizeof(Type)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_weight), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_weight, p->p_weight, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_delay_steps), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_delay_steps, p->p_delay_steps, sizeof(int)*p->num, cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_delay), sizeof(real)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->p_delay, p->p_delay, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_C1), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_C1, p->p_C1, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__C1), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__C1, p->p__C1, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_tau_syn), sizeof(real)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->p_tau_syn, p->p_tau_syn, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_I_syn), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_I_syn, p->p_I_syn, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__dt), sizeof(real)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->p__dt, p->p__dt, sizeof(real)*p->num, cudaMemcpyHostToDevice));

	//checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pSrc), sizeof(int)*p->num));
	//checkCudaErrors(cudaMemcpy(pGpuSynapses->pSrc, p->pSrc, sizeof(int)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pDst), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pDst, p->pDst, sizeof(int)*p->num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeExpSynapses(void *pGpu)
{
	GExpSynapses *pGpuSynapses = (GExpSynapses*)pGpu;

	//checkCudaErrors(cudaFree(pGpuSynapses->pID));

	//checkCudaErrors(cudaFree(pGpuSynapses->pType));

	checkCudaErrors(cudaFree(pGpuSynapses->p_weight));
	checkCudaErrors(cudaFree(pGpuSynapses->p_delay_steps));
	//checkCudaErrors(cudaFree(pGpuSynapses->p_delay));
	checkCudaErrors(cudaFree(pGpuSynapses->p_C1));
	checkCudaErrors(cudaFree(pGpuSynapses->p__C1));
	//checkCudaErrors(cudaFree(pGpuSynapses->p_tau_syn));
	checkCudaErrors(cudaFree(pGpuSynapses->p_I_syn));
	//checkCudaErrors(cudaFree(pGpuSynapses->p__dt));

	//checkCudaErrors(cudaFree(pGpuSynapses->pSrc));
	checkCudaErrors(cudaFree(pGpuSynapses->pDst));

	return 0;
}
