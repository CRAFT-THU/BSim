
#include "../../gpu_utils/runtime_info.h"

#include "GStatic.h"

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id)
{
#define FAST_TEST 2
#if  FAST_TEST == 1
	__shared__ int fire_neuron_id[MAXBLOCKSIZE];

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		int block_nums_minus_1 = (firedSize - 1 + blockDim.x) / blockDim.x - 1;
		int grid_nums = (firedSize - 1 + blockDim.x*gridDim.x)/(blockDim.x * gridDim.x);
		int oid = tid;
		for (int idx = 0; idx < grid_nums; idx++) {
			if (oid < firedSize) {
				fire_neuron_id[threadIdx.x] = gFiredTable[time_idx*gFiredTableCap + oid];
			} else {
				fire_neuron_id[threadIdx.x] = -1;
			}
			oid += blockDim.x * gridDim.x;
			__syncthreads();

			int size = 0;
			if (block_idx == block_nums_minus_1) {
				size = firedSize - block_idx * blockDim.x;
			} else if (block_idx < block_nums_minus_1) {
				size = blockDim.x;
			} else {
				size = 0;
			}

			for (int i=0; i<size; i++) {
				int nid = fire_neuron_id[i];
				int start_loc = gConnection->delayStart[delta_t + nid * MAX_DELAY];
				int synapseNum = gConnection->delayNum[delta_t + nid * MAX_DELAY];
				gLayerInput[nid]++;
				for (int j=threadIdx.x; j<synapseNum; j += blockDim.x) {
					//int sid = gConnection->pSynapsesIdx[j+start_loc];
					int sid = j+start_loc;
					real weight = d_synapses->p_weight[sid];
					if (weight >= 0) {
						atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
					} else {
						atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
					}
				}
			}
			block_idx += gridDim.x;
			__syncthreads();
		}
		__syncthreads();
	}
#elif FAST_TEST == 2
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		int num_per_block = (firedSize - 1)/gridDim.x + 1;
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
			int nid = gFiredTable[time_idx*gFiredTableCap + (block_idx)*num_per_block + idx];
			int start_loc = gConnection->delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = gConnection->delayNum[delta_t + nid * MAX_DELAY];
			if (threadIdx.x == 0) {
				gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j<synapseNum; j += blockDim.x) {
				//int sid = gConnection->pSynapsesIdx[j+start_loc];
				int sid = j+start_loc;
				real weight = d_synapses->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
				}
			}
		}
		__syncthreads();
	}
#else
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
			int nid = gFiredTable[time_idx*gFiredTableCap + idx];
			int start_loc = gConnection->delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = gConnection->delayNum[delta_t + nid * MAX_DELAY];
			gLayerInput[nid]++;
			for (int i=0; i<synapseNum; i++) {
				//int sid = gConnection->pSynapsesIdx[i+start_loc];
				int sid = i+start_loc;
				real weight = d_synapses->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
				}
			}
		}
	}
#endif
}
