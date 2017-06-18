/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../third_party/cuda/helper_cuda.h"
#include "GTJNeurons.h"

int cudaAllocTJ(void *pCpu, void *pGpu, int num)
{
	GTJNeurons *pGpuNeurons = (GTJNeurons*)pGpu;
	GTJNeurons *p = (GTJNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_vm), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_vm, p->p_vm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_reset), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_reset, p->p_v_reset, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_thresh), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_thresh, p->p_v_thresh, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_cm, p->p_cm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_cm, p->p_cm, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_step), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_step, p->p_refrac_step, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_time), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_time, p->p_refrac_time, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeTJ(void *pGpu)
{
	GTJNeurons *pGpuNeurons = (GTJNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->p_vm));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_thresh));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_reset));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_tmp));
	checkCudaErrors(cudaFree(pGpuNeurons->p_refrac_step));
	checkCudaErrors(cudaFree(pGpuNeurons->p_refrac_time));

	return 0;
}
