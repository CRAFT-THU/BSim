#include "runtime.h"
#include "connection.h"

__global__ void find_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id);;
__global__ void update_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id);;

extern "C" {
	void update_stdp_synapse(Stdp_synapse *data, int num, int start_id, int t)
	{
		update_stdp_synapse_gpu<<<GRIDSIZE, BLOCKSIZE>>>((Stdp_synapse*)data, num, start_id, t);
		learn_stdp_synapse_post<<<GRIDSIZE, BLOCKSIZE>>>((Stdp_synapse*)data, num, start_id, t);
	}
}

__device__ void _clip(float a, float min, float max)
{
	if (a < min) {
		return min;
	} else if (a > max) {
		return max;
	} else {
		return a;
	}
}
__global__ void update_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id, int t)
{
	for (int delta_t=MIN_DELAY; delta_t<=MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (t + MAX_DELAY - delta_t) % ( MAX_DELAY + 1);
		int firedSize = gFiredTableSizes[time_idx];
		int num_per_block = (firedSize - 1) / gridDim.x + 1;
		int block_nums_minus_1 = (firedSize - 1) / num_per_block;
		int fired_size_block = 0;
		if (block_idx == block_nums_minus_1) {
		fired_size_block = firedSize - block_idx * num_per_block;
		} else if (block_idx < block_nums_minus_1) {
		fired_size_block = num_per_block;
		} else {
		fired_size_block = 0;
		}
		for (int idx = 0; idx < fired_size_block; idx++) {
			int nid = gFiredTable[time_idx * gFiredTableCap + (block_idx) * num_per_block + idx];
			int start_loc = connection->delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = connection->delayNum[delta_t + nid * MAX_DELAY];
			if (threadIdx.x == 0) {
				gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {
				int sid = j+start_loc;
				real weight = d_synapses->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(gNeuronInput[datas->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(gNeuronInput_I[datas->p_dst[sid]]), weight);
				}

				data->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));
				data->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));

				data->p_pre[sid] += data->p_d_apre[sid];
				data->p_weight[sid] = _clip(weight + data->p_apost[sid], GMIN, GMAX);
				data->p_last_update[sid] = gCurrentCycle;
			}
		}
		__syncthreads();
	}
}

__global__ void update_stdp_synapse_gpu(Stdp_synapse * data, int num, int start_id, int t)
{
	int block_idx = blockIdx.x;
	int time_idx = t%(MAX_DELAY+1);
	int firedSize = gFiredTableSizes[time_idx];
	int num_per_block = (firedSize - 1) / gridDim.x + 1;
	int block_nums_minus_1 = (firedSize - 1) / num_per_block;
	int fired_size_block = 0;
	if (block_idx == block_nums_minus_1) {
	fired_size_block = firedSize - block_idx * num_per_block;
	} else if (block_idx < block_nums_minus_1) {
	fired_size_block = num_per_block;
	} else {
		fired_size_block = 0;
	}

	for (int idx = 0; idx < fired_size_block; idx++) {
		int nid = gFiredTable[time_idx * gFiredTableCap + (block_idx) * num_per_block + idx];
		int start_loc = gConnectionStdp_synapse->rev_delayStart[nid];
		int synapseNum = gConnectionStdp_synapse->rev_delayNum[nid];
		for (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {
			int sid = connection->rev_map2sid[j+start_loc];
			data->p_apre[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_pre[sid]));
			data->p_apost[sid] *= exp((data->p_last_update[sid] - t) / (data->p_tau_post[sid]));
			data->p_apost[sid] += data->p_d_apost[sid];
			data->p_weight[sid] = _clip(weight + data->p_pre[sid], GMIN, GMAX);
			data->p_last_update[sid] = gCurrentCycle;
		}
	}
	__syncthreads();
}

