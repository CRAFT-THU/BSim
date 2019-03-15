

#include <stdlib.h>

#include "../c_code/helper_cuda.h"
#include "static.h"


Static * to_gpu_static(Static *cpu, int num)
{
	Static * gpu = static_cast<Static*>(malloc(sizeof(Static)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_dst), sizeof(int)*num));
	checkCudaErrors(cudaMemset(gpu->p_dst, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_dst, cpu->p_dst, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_weight), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_weight, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_weight, cpu->p_weight, sizeof(double)*num, cudaMemcpyHostToDevice));
	Static * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Static)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(Static)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Static)*1, cudaMemcpyHostToDevice));
	return ret;
}

Static * from_gpu_static(Static *gpu, int num)
{
	Static * cpu = static_cast<Static*>(malloc(sizeof(Static)*1));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(Static)*1, cudaMemcpyDeviceToHost));
	return cpu;
}

