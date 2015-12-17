/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */

#include "utils/cuda/helper_cuda.h"
#include "GAlphaSynapses.h"

int GAlphaSynapses::allocGSynapses(GAlphaSynapses *pGpuSynapses)
{
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pID), sizeof(ID)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pID, pID, sizeof(ID)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pType), sizeof(NeuronType)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pType, pType, sizeof(NeuronType)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_weight), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_weight, p_weight, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_delay), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_delay, p_delay, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_C1, p_C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_C2), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_C2, p_C2, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__C1, p__C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__C2), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__C2, p__C2, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_tau_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_tau_syn, p_tau_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_I_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_I_syn, p_I_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_I_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_I_tmp, p_I_tmp, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__dt), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__dt, p__dt, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pSrc), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pSrc, pSrc, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pDst), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pDst, pDst, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int freeGSynapses(GAlphaSynapses *pGpuSynapses)
{
	checkCudaErrors(cudaFree(pGpuSynapses->pID));

	checkCudaErrors(cudaFree(pGpuSynapses->pType));

	checkCudaErrors(cudaFree(pGpuSynapses->p_weight));
	checkCudaErrors(cudaFree(pGpuSynapses->p_delay));
	checkCudaErrors(cudaFree(pGpuSynapses->p_C1));
	checkCudaErrors(cudaFree(pGpuSynapses->p_C2));
	checkCudaErrors(cudaFree(pGpuSynapses->p__C1));
	checkCudaErrors(cudaFree(pGpuSynapses->p__C2));
	checkCudaErrors(cudaFree(pGpuSynapses->p_tau_syn));
	checkCudaErrors(cudaFree(pGpuSynapses->p_I_syn));
	checkCudaErrors(cudaFree(pGpuSynapses->p_I_tmp));
	checkCudaErrors(cudaFree(pGpuSynapses->p__dt));

	checkCudaErrors(cudaFree(pGpuSynapses->pSrc));
	checkCudaErrors(cudaFree(pGpuSynapses->pDst));

	return 0;
}
