#include "runtime.h"
#include "static.h"


__device__ double _clip(double a, double min, double max)
{
	if (a < min) {
		return min;
	} else if (a > max) {
		return max;
	} else {
		return a;
	}
}
__global__ void update_static_gpu(Static *data, int num, int start_id, int t)
{
	for (int delta_t=MIN_DELAY; delta_t<=MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int delay_idx = (t + MAX_DELAY + 1 - delta_t) % ( MAX_DELAY + 1);
		int fired_size = g_fired_table_sizes[delay_idx];
		int num_per_block = (fired_size - 1) / gridDim.x + 1;
		int block_nums_minus_1 = (fired_size - 1) / num_per_block;
		int fired_size_block = 0;
		if (block_idx == block_nums_minus_1) {
		fired_size_block = fired_size - block_idx * num_per_block;
		} else if (block_idx < block_nums_minus_1) {
		fired_size_block = num_per_block;
		} else {
		fired_size_block = 0;
		}
		for (int idx = 0; idx < fired_size_block; idx++) {
			int nid = g_fired_table[delay_idx * FIRED_TABLE_SIZE + (block_idx) * num_per_block + idx];
			int start_loc = g_connection_static->delay_start[delta_t - MIN_DELAY + nid * (MAX_DELAY - MIN_DELAY + 1)];
			int synapse_num = g_connection_static->delay_num[delta_t - MIN_DELAY + nid * (MAX_DELAY - MIN_DELAY + 1)];
			for (int j=threadIdx.x; j < synapse_num; j += blockDim.x) {
				int sid = j+start_loc;
				double weight = data->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(g_i_exec[data->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(g_i_inh[data->p_dst[sid]]), weight);
				}

			}
		}
		__syncthreads();
	}
}

void update_static(Static *data, int num, int start_id, int t)
{
	update_static_gpu<<<STATIC_GRID_SIZE, STATIC_BLOCK_SIZE>>>((Static*)data, num, start_id, t);
}

