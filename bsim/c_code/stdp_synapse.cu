extern "C" {
	void update_stdp_synapse(Stdp_synapse *data, int num, int start_id, BlockSize *size)
	{
		update_stdp_synapse_gpu<<<size=>gridSize, size->blockSize>>>((Stdp_synapse*)data, num, start_id);
		learn_stdp_synapse_post<<<size=>gridSize, size->blockSize>>>((Stdp_synapse*)data, num, start_id);
	}
}

__global void update_stdp_synapse_gpu(stdp_synapse *data, int num, int start_id)
{
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (gCurrentIdx + MAX_DELAY - delta_t) % ( MAX_DELAY + 1);
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
			int start_loc = gConnection->rev_delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = gConnection->rev_delayNum[delta_t + nid * MAX_DELAY];
			if (threadIdx.x == 0) {
				gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {
				int sid = j+start_loc;
				real weight = d_synapses->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
				}

				d_synapse->p_apre[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_pre[sid]));
				d_synapse->p_apost[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_post[sid]));

				d_synapse->p_pre[sid] += d_synapse->p_d_apre[sid];
				d_synapse->p_weight[sid] = _clip(weight + d_synapse->p_apost[sid], gMin, gMax);
				d_synapse->p_last_update[sid] = gCurrentCycle;
			}
		}
		__syncthreads();
	}
}

__global void update_stdp_synapse_gpu(stdp_synapse *data, int num, int start_id)
{
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (gCurrentIdx + MAX_DELAY - delta_t) % (MAX_DELAY + 1);
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
			int start_loc = gConnection->rev_delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = gConnection->rev_delayNum[delta_t + nid * MAX_DELAY];
			if (threadIdx.x == 0) {
			gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j < synapseNum; j += blockDim.x) {
			int sid = gConnection->rev_map2sid[j+start_loc];

			d_synapse->p_apre[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_pre[sid]));
			d_synapse->p_apost[sid] *= exp((d_synapse->p_last_update[sid] - t) / (d_synapse->p_tau_post[sid]));
			d_synapse->p_apost[sid] += d_synapse->p_d_apost[sid];
			d_synapse->p_weight[sid] = _clip(weight + d_synapse->p_pre[sid], gMin, gMax);
			d_synapse->p_last_update[sid] = gCurrentCycle;
			}
		}
		__syncthreads();
	}
}

