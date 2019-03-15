#include <stdio.h>
#include "../c_code/helper_cuda.h"
#include "runtime.h"


__device__ int * g_fired_table;
__device__ int * g_fired_table_sizes;
__device__ int * g_active_lif_curr_exp_table;
__device__ int g_active_lif_curr_exp_table_size;
__device__ CConnection * g_connection_static;
__device__ double * i_exec;
__device__ double * g_i_exec;
__device__ double * g_i_inh;
__device__ double * i_inh;


void **init_runtime(CConnection ** connections)
{
	int zero = 0;
	int *p_int = NULL;
	double *p_double = NULL;

	void **ret = static_cast<void**>(malloc(sizeof(void*) * 2));

	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*2));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*2));
	checkCudaErrors(cudaMemcpyToSymbol(g_fired_table_sizes, &p_int, sizeof(p_int)));
	ret[0] = static_cast<void*>(p_int);
	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*60));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*60));
	checkCudaErrors(cudaMemcpyToSymbol(g_fired_table, &p_int, sizeof(p_int)));
	ret[1] = static_cast<void*>(p_int);

	checkCudaErrors(cudaMemcpyToSymbol(g_active_lif_curr_exp_table_size, &zero, sizeof(int)));
	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*30));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*30));
	checkCudaErrors(cudaMemcpyToSymbol(g_active_lif_curr_exp_table, &p_int, sizeof(p_int)));


	checkCudaErrors(cudaMalloc((void**)&(p_double), sizeof(double)*30));
	checkCudaErrors(cudaMemset(p_double, 0, sizeof(double)*30));
	checkCudaErrors(cudaMemcpyToSymbol(i_exec, &p_double, sizeof(p_double)));
	checkCudaErrors(cudaMalloc((void**)&(p_double), sizeof(double)*30));
	checkCudaErrors(cudaMemset(p_double, 0, sizeof(double)*30));
	checkCudaErrors(cudaMemcpyToSymbol(g_i_exec, &p_double, sizeof(p_double)));
	checkCudaErrors(cudaMalloc((void**)&(p_double), sizeof(double)*30));
	checkCudaErrors(cudaMemset(p_double, 0, sizeof(double)*30));
	checkCudaErrors(cudaMemcpyToSymbol(g_i_inh, &p_double, sizeof(p_double)));
	checkCudaErrors(cudaMalloc((void**)&(p_double), sizeof(double)*30));
	checkCudaErrors(cudaMemset(p_double, 0, sizeof(double)*30));
	checkCudaErrors(cudaMemcpyToSymbol(i_inh, &p_double, sizeof(p_double)));
	checkCudaErrors(cudaMemcpyToSymbol(g_connection_static, &(connections[0]), sizeof(CConnection*)));
	return ret;
}
