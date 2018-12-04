

#include <stdlib.h>

#include "helper_cuda.h"
#include "stdp_synapse.h"


Stdp_synapse * to_gpu_stdp_synapse(Stdp_synapse *cpu, int num)
{
	Stdp_synapse * gpu = static_cast<Stdp_synapse*>(malloc(sizeof(Stdp_synapse)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_last_update), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_last_update, cpu->p_last_update, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_delay), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_delay, cpu->p_delay, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_dst), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_dst, cpu->p_dst, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_tau_post), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_tau_post, cpu->p_tau_post, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_d_apost), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_d_apost, cpu->p_d_apost, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_tau_pre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_tau_pre, cpu->p_tau_pre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_d_apre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_d_apre, cpu->p_d_apre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_weight), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_weight, cpu->p_weight, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_apre), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_apre, cpu->p_apre, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_apost), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_apost, cpu->p_apost, sizeof(float)*num, cudaMemcpyHostToDevice));
	Stdp_synapse * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Stdp_synapse)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Stdp_synapse)*1, cudaMemcpyHostToDevice));
	return ret;
}

Stdp_synapse * from_gpu_stdp_synapse(Stdp_synapse *gpu, int num)
{
	Stdp_synapse * cpu = static_cast<Stdp_synapse*>(malloc(sizeof(Stdp_synapse)*1));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(Stdp_synapse)*1, cudaMemcpyDeviceToHost));
	return cpu;
}

