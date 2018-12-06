
#include "curand_kernel.h"
#include "curand.h"
#include "helper_cuda.h"
#include "runtime.h"

__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, int *global_buf, int * global_size, int offset)
{
	__shared__ volatile unsigned int start_loc;
	if (threadIdx.x == 0) {
		start_loc = atomicAdd(global_size, (int)size);
	}
	__syncthreads();

	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		global_buf[offset + start_loc + idx] = shared_buf[idx];
	}

	return 0;
}

__global__ void curand_setup_kernel(curandState *state, int num)
{
	int id = threadIdx.x + blockIdx.x * blockDim.x;
	if (id < num) {
		curand_init(1234, id, 0, &state[id]);
	}
}


__global__ void add_cross_neuron(int *ids, int num, int t)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delay_idx = t % (MAX_DELAY+1);
	if (tid < num) {
		g_fired_table[FIRED_TABLE_SIZE*delay_idx + g_fired_table_sizes[delay_idx] + tid] = ids[tid];
	}
	__syncthreads();

	if (tid == 0) {
		g_fired_table_sizes[delay_idx] += num;
	}
}

__global__ void deliver_neurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num, int t)
{
	__shared__ int cross_neuron_id[MAX_BLOCK_SIZE];
	__shared__ volatile int cross_cnt;

	if (threadIdx.x == 0) {
		cross_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delay_idx = t % (MAX_DELAY+1);
	int fired_size = g_fired_table_sizes[delay_idx];
	for (int node = 0; node < node_num; node++) {
		for (int idx = tid; idx < fired_size; idx += blockDim.x * gridDim.x) {
			int nid = g_fired_table[FIRED_TABLE_SIZE*delay_idx+ idx];
			int tmp = idx2index[nid];
			if (tmp >= 0) {
				int map_nid = crossnode_index2idx[tmp*node_num + node];
				if (map_nid >= 0) {
					int test_loc = atomicAdd((int*)&cross_cnt, 1);
					if (test_loc < MAX_BLOCK_SIZE) {
						cross_neuron_id[test_loc] = map_nid;
					}
				}
			}
			__syncthreads();

			if (cross_cnt > 0) {
				commit2globalTable(cross_neuron_id, cross_cnt, global_cross_data, &fired_n_num[node], FIRED_TABLE_SIZE*node);
				if (threadIdx.x == 0) {
					cross_cnt = 0;
				}
			}
			__syncthreads();
		}
		__syncthreads();
	}
}