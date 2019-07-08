#include "runtime.h"
#include "stdp_synapse.h"


__global__ void update_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id, int t)
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
			int start_loc = g_connection_stdp_synapse->delay_start[delta_t - MIN_DELAY + nid * (MAX_DELAY - MIN_DELAY + 1)];
			int synapse_num = g_connection_stdp_synapse->delay_num[delta_t - MIN_DELAY + nid * (MAX_DELAY - MIN_DELAY + 1)];
			for (int j=threadIdx.x; j < synapse_num; j += blockDim.x) {
				int sid = j+start_loc;
				double weight = data->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(g_i_exec[data->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(g_i_inh[data->p_dst[sid]]), weight);
				}

				data->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));
				data->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));

				data->p_apre[sid] += data->p_d_apre[sid];
				data->p_weight[sid] = _clip(weight + data->p_apost[sid], G_MIN, G_MAX);
				data->p_last_update[sid] = t;
			}
		}
		__syncthreads();
	}
}

__global__ void learn_stdp_synapse_post(Stdp_synapse * data, int num, int start_id, int t)
{
	int block_idx = blockIdx.x;
	int delay_idx = t%(MAX_DELAY+1);
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
		int start_loc = g_connection_stdp_synapse->rev_delay_start[nid];
		int synapse_num = g_connection_stdp_synapse->rev_delay_num[nid];
		for (int j=threadIdx.x; j<synapse_num; j+=blockDim.x) {
			int sid = g_connection_stdp_synapse->rev_map2sid[j+start_loc];
			data->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));
			data->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));
			data->p_apost[sid] += data->p_d_apost[sid];
			data->p_weight[sid] = _clip(data->p_weight[sid] + data->p_apre[sid], G_MIN, G_MAX);
			data->p_last_update[sid] = t;
		}
	}
	__syncthreads();
}

void update_stdp_synapse(Stdp_synapse *data, int num, int start_id, int t)
{
	update_stdp_synapse_gpu<<<STDP_SYNAPSE_GRID_SIZE, STDP_SYNAPSE_BLOCK_SIZE>>>((Stdp_synapse*)data, num, start_id, t);
	learn_stdp_synapse_post<<<STDP_SYNAPSE_GRID_SIZE, STDP_SYNAPSE_BLOCK_SIZE>>>((Stdp_synapse*)data, num, start_id, t);
}

