#include "helper_cuda.h"
#include "runtime.h"


void init_runtime(CConnection ** connections)
{
	int zero = 0;
	int *p_int = NULL;
	float *p_float = NULL;

	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*24));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*24));
	checkCudaErrors(cudaMemcpyToSymbol(g_fired_table, &p_int, sizeof(p_int)));
	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*3));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*3));
	checkCudaErrors(cudaMemcpyToSymbol(g_fired_table_sizes, &p_int, sizeof(p_int)));

	checkCudaErrors(cudaMalloc((void**)&(p_int), sizeof(int)*8));
	checkCudaErrors(cudaMemset(p_int, 0, sizeof(int)*8));
	checkCudaErrors(cudaMemcpyToSymbol(g_active_lif_curr_exp_table, &p_int, sizeof(p_int)));
	checkCudaErrors(cudaMemcpyToSymbol(g_active_lif_curr_exp_table_size, &zero, sizeof(int)));


	checkCudaErrors(cudaMalloc((void**)&(p_float), sizeof(float)*8));
	checkCudaErrors(cudaMemset(p_float, 0, sizeof(float)*8));
	checkCudaErrors(cudaMemcpyToSymbol(g_i_exec, &p_float, sizeof(p_float)));
	checkCudaErrors(cudaMalloc((void**)&(p_float), sizeof(float)*8));
	checkCudaErrors(cudaMemset(p_float, 0, sizeof(float)*8));
	checkCudaErrors(cudaMemcpyToSymbol(g_i_inh, &p_float, sizeof(p_float)));
	checkCudaErrors(cudaMemcpyToSymbol(g_connection_stdp_synapse, &(connections[0]), sizeof(CConnection*)));
}
