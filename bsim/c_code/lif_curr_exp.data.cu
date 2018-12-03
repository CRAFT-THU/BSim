

#include <stdlib.h>

#include "helper_cuda.h"
#include "lif_curr_exp.h"


Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num)
{
	Lif_curr_exp * gpu = static_cast<Lif_curr_exp*>(malloc(sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_refract_time), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_refract_time, cpu->p_refract_time, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_refract_step), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_refract_step, cpu->p_refract_step, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_C_exec), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_C_exec, cpu->p_C_exec, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_C_inh), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_C_inh, cpu->p_C_inh, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_tmp), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_tmp, cpu->p_v_tmp, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_Cm), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_Cm, cpu->p_Cm, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_Cinh), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_Cinh, cpu->p_Cinh, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_threshold), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_threshold, cpu->p_v_threshold, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_Cexec), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_Cexec, cpu->p_Cexec, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_reset), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_reset, cpu->p_v_reset, sizeof(float)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v, cpu->p_v, sizeof(float)*num, cudaMemcpyHostToDevice));
	Lif_curr_exp * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Lif_curr_exp)*1, cudaMemcpyHostToDevice));
	return ret;
}

Lif_curr_exp * from_gpu_lif_curr_exp(Lif_curr_exp *gpu, int num)
{
	Lif_curr_exp * cpu = static_cast<Lif_curr_exp*>(malloc(sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(Lif_curr_exp)*1, cudaMemcpyDeviceToHost));
	return cpu;
}

