

#include <stdlib.h>

#include "helper_cuda.h"
#include "connection.h"


CConnection * to_gpu_connection(CConnection *cpu)
{
	CConnection * gpu = (CConnection*)malloc(sizeof(CConnection));
	gpu->n_len = cpu->n_len;
	gpu->r_n_len = cpu->r_n_len;
	gpu->s_len = cpu->s_len;
	checkCudaErrors(cudaMalloc((void**)&(gpu->delay_start), sizeof(int)*cpu->n_len));
	checkCudaErrors(cudaMemset(gpu->delay_start, 0, sizeof(int)*cpu->n_len));
	checkCudaErrors(cudaMemcpy(gpu->delay_start, cpu->delay_start, sizeof(int)*cpu->n_len, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->delay_num), sizeof(int)*cpu->n_len));
	checkCudaErrors(cudaMemset(gpu->delay_num, 0, sizeof(int)*cpu->n_len));
	checkCudaErrors(cudaMemcpy(gpu->delay_num, cpu->delay_num, sizeof(int)*cpu->n_len, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_delay_start), sizeof(int)*cpu->r_n_len));
	checkCudaErrors(cudaMemset(gpu->rev_delay_start, 0, sizeof(int)*cpu->r_n_len));
	checkCudaErrors(cudaMemcpy(gpu->rev_delay_start, cpu->rev_delay_start, sizeof(int)*cpu->r_n_len, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_delay_num), sizeof(int)*cpu->r_n_len));
	checkCudaErrors(cudaMemset(gpu->rev_delay_num, 0, sizeof(int)*cpu->r_n_len));
	checkCudaErrors(cudaMemcpy(gpu->rev_delay_num, cpu->rev_delay_num, sizeof(int)*cpu->r_n_len, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(gpu->rev_map2sid), sizeof(int)*cpu->s_len));
	checkCudaErrors(cudaMemset(gpu->rev_map2sid, 0, sizeof(int)*cpu->s_len));
	checkCudaErrors(cudaMemcpy(gpu->rev_map2sid, cpu->rev_map2sid, sizeof(int)*cpu->s_len, cudaMemcpyHostToDevice));
	CConnection * ret = NULL;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(CConnection)*1));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(CConnection)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(CConnection)*1, cudaMemcpyHostToDevice));
	return ret;
}

CConnection * from_gpu_connection(CConnection *gpu)
{
	CConnection * ret = static_cast<CConnection*>(malloc(sizeof(CConnection)*1));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(CConnection)*1, cudaMemcpyDeviceToHost));
	return ret;
}

