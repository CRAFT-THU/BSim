/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */

#include "utils/cuda/helper_cuda.h"
#include "GExpSynapses.h"

int GExpSynapses::allocGSynapses(GExpSynapses *pGpuSynapses)
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
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__C1, p__C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_tau_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_tau_syn, p_tau_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_I_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_I_syn, p_I_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p__dt), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p__dt, p__dt, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pSrc), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pSrc, pSrc, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pDst), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pDst, pDst, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));

	return 0;
}
