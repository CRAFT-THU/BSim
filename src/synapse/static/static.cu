
#include "../../gpu_utils/runtime_info.h"

// #include "GStaticSynapses.h"
#include "static.h"


__global__ void update_dense_static_hit(Connection *connection, GStaticSynapses *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time)
{
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delayLength = connection->maxDelay - connection->minDelay + 1;
	for (int delta_t = 0; delta_t<delayLength; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (time - connection->minDelay - delta_t)%(delayLength);
		int firedSize = firedTableSizes[time_idx];
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
			int nid = firedTable[time_idx*gFiredTableCap + (block_idx)*num_per_block + idx];
			int start_loc = connection->pDelayStart[delta_t + nid * delayLength];
			int synapseNum = connection->pDelayNum[delta_t + nid * delayLength];
			if (threadIdx.x == 0) {
				gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j<synapseNum; j += blockDim.x) {
				//int sid = connection->pSynapsesIdx[j+start_loc];
				int sid = j+start_loc;
				real weight = data->pWeight[sid];
				if (weight >= 0) {
					atomicAdd(&(currentE[data->pDst[sid]]), weight);
				} else {
					atomicAdd(&(currentI[data->pDst[sid]]), weight);
				}
			}
		}
		__syncthreads();
	}
}

void cudaUpdateStatic(void * connection, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time, BlockSize *pSize)
{
	//update_static_hit<<<pSize->gridSize, pSize->blockSize>>>((GStaticSynapses*)data, num, start_id);
	//reset_active_synapse<<<1, 1>>>();
	update_dense_static_hit<<<pSize->gridSize, pSize->blockSize>>>((Connection *)connection,  (GStaticSynapses *)data, currentE, currentI, firedTable, firedTableSizes, num, start_id, time);

}

