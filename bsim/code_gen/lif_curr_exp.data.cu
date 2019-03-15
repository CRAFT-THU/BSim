

#include <stdlib.h>

#include "../c_code/helper_cuda.h"
#include "lif_curr_exp.h"


Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num)
{
	Lif_curr_exp * gpu = static_cast<Lif_curr_exp*>(malloc(sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_refract_time), sizeof(int)*num));
	checkCudaErrors(cudaMemset(gpu->p_refract_time, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_refract_time, cpu->p_refract_time, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_refract_step), sizeof(int)*num));
	checkCudaErrors(cudaMemset(gpu->p_refract_step, 0, sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_refract_step, cpu->p_refract_step, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_Cinh), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_Cinh, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_Cinh, cpu->p_Cinh, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_tmp), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_v_tmp, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_tmp, cpu->p_v_tmp, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_reset), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_v_reset, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_reset, cpu->p_v_reset, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_Cexec), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_Cexec, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_Cexec, cpu->p_Cexec, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_C_inh), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_C_inh, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_C_inh, cpu->p_C_inh, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_C), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_C, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_C, cpu->p_C, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_C_exec), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_C_exec, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_C_exec, cpu->p_C_exec, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v_threshold), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_v_threshold, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v_threshold, cpu->p_v_threshold, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_i_exec), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_i_exec, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_i_exec, cpu->p_i_exec, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_v, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v, cpu->p_v, sizeof(double)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_i_inh), sizeof(double)*num));
	checkCudaErrors(cudaMemset(gpu->p_i_inh, 0, sizeof(double)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_i_inh, cpu->p_i_inh, sizeof(double)*num, cudaMemcpyHostToDevice));
	Lif_curr_exp * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Lif_curr_exp)*1, cudaMemcpyHostToDevice));
	return ret;
}

Lif_curr_exp * from_gpu_lif_curr_exp(Lif_curr_exp *gpu, int num)
{
	Lif_curr_exp * cpu = static_cast<Lif_curr_exp*>(malloc(sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(Lif_curr_exp)*1, cudaMemcpyDeviceToHost));
	return cpu;
}

