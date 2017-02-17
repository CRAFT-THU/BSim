/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../third_party/cuda/helper_cuda.h"
#include "GLIFNeurons.h"

int cudaAllocLIF(void *pCpu, void *pGpu, int num)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;
	GLIFNeurons *p = (GLIFNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_reset), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_reset, p->p_v_reset, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_thresh), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_thresh, p->p_v_thresh, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_syn, p->p_i_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_vm), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_vm, p->p_vm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C1, p->p_C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C2), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C2, p->p_C2, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_tmp, p->p_i_tmp, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_step), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_step, p->p_refrac_step, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_time), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_time, p->p_refrac_time, sizeof(int)*num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeLIF(void *pGpu)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->p_v_reset));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_thresh));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_syn));
	checkCudaErrors(cudaFree(pGpuNeurons->p_vm));
	checkCudaErrors(cudaFree(pGpuNeurons->p_C1));
	checkCudaErrors(cudaFree(pGpuNeurons->p_C2));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_tmp));
	checkCudaErrors(cudaFree(pGpuNeurons->p_refrac_step));
	checkCudaErrors(cudaFree(pGpuNeurons->p_refrac_time));

	return 0;
}
