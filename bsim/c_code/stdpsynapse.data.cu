

#include <stdlib.h>

#include "helper_cuda.h"
#include "stdpsynapse.h"


Stdpsynapse * to_gpu_stdpsynapse(Stdpsynapse *cpu, int num)
{
	Stdpsynapse * gpu = static_cast<Stdpsynapse*>(malloc(sizeof(Stdpsynapse)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_last_update), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_last_update, cpu->p_last_update, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_dst), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_dst, cpu->p_dst, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_delay), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_delay, cpu->p_delay, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_tau_pre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_tau_pre, cpu->p_tau_pre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_tau_post), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_tau_post, cpu->p_tau_post, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_d_apre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_d_apre, cpu->p_d_apre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_d_apost), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_d_apost, cpu->p_d_apost, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_apre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_apre, cpu->p_apre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_weight), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_weight, cpu->p_weight, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_apost), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_apost, cpu->p_apost, sizeof(float)*num, cudaMemcpyHostToDevice));
	Stdpsynapse * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Stdpsynapse)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Stdpsynapse)*1, cudaMemcpyHostToDevice));
	return ret;
}

Stdpsynapse * from_gpu_stdpsynapse(Stdpsynapse *gpu, int num)
{
	Stdpsynapse * cpu = static_cast<Stdpsynapse*>(malloc(sizeof(Stdpsynapse)*1));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(Stdpsynapse)*1, cudaMemcpyDeviceToHost));
	return cpu;
}

