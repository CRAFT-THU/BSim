

#include <stdlib.h>

#include "helper_cuda.h"
#include "connection.h"


CConnection * to_gpu_connection(CConnection *cpu)
{
	CConnection * gpu = (CConnection*)malloc(sizeof(CConnection));
	gpu->n_length = cpu->n_length;
	gpu->s_length = cpu->s_length;
	checkCudaErrors(cudaMalloc((void**)&(gpu->delay_start), sizeof(int)*cpu->n_length));
	checkCudaErrors(cudaMemcpy(gpu->delay_start, cpu->delay_start, sizeof(int)*cpu->n_length, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->delay_num), sizeof(int)*cpu->n_length));
	checkCudaErrors(cudaMemcpy(gpu->delay_num, cpu->delay_num, sizeof(int)*cpu->n_length, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_delay_start), sizeof(int)*cpu->n_length));
	checkCudaErrors(cudaMemcpy(gpu->rev_delay_start, cpu->rev_delay_start, sizeof(int)*cpu->n_length, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_delay_num), sizeof(int)*cpu->n_length));
	checkCudaErrors(cudaMemcpy(gpu->rev_delay_num, cpu->rev_delay_num, sizeof(int)*cpu->n_length, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_map2sid), sizeof(int)*cpu->s_length));
	checkCudaErrors(cudaMemcpy(gpu->rev_map2sid, cpu->rev_map2sid, sizeof(int)*cpu->s_length, cudaMemcpyHostToDevice));
	CConnection * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(CConnection)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(CConnection)*1, cudaMemcpyHostToDevice));
	return ret;
}

CConnection * from_gpu_connection(CConnection *gpu)
{
	CConnection * ret = static_cast<CConnection*>(malloc(sizeof(CConnection)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(CConnection)*1, cudaMemcpyDeviceToHost));
	return ret;
}

