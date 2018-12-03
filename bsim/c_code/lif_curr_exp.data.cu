

#include <stdlib.h>

#include "helper_cuda.h"
#include "lif_curr_exp.h"


Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num)
{
	Lif_curr_exp * gpu = static_cast<Lif_curr_exp*>(malloc(sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_refract_step), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_refract_step, cpu->p_refract_step, sizeof(int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->p_v), sizeof(float)*num));
	checkCudaErrors(cudaMemcpy(gpu->p_v, cpu->p_v, sizeof(float)*num, cudaMemcpyHostToDevice));
	Lif_curr_exp * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(Lif_curr_exp)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(Lif_curr_exp)*1, cudaMemcpyHostToDevice));
	return ret;
}

