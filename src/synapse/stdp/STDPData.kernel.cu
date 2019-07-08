
#include "../../gpu_utils/runtime.h"

#include "STDPData.h"


__global__ void update_stdp_hit(Connection *connection, STDPData *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time)
{
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delayLength = connection->maxDelay - connection->minDelay + 1;
	for (int delta_t = 0; delta_t<delayLength; delta_t++) {
		int block_idx = blockIdx.x;
		int timeIdx = (time+delayLength-delta_t)%(connection->maxDelay+1);
		int firedSize = firedTableSizes[timeIdx];
		int numPerBlock = (firedSize - 1)/gridDim.x + 1;
		int blockNumMinusOne = (firedSize - 1) / numPerBlock;

		int firedSizeBlock = 0;
		if (block_idx == blockNumMinusOne) {
			firedSizeBlock = firedSize - block_idx * numPerBlock;
		} else if (block_idx < blockNumMinusOne) {
			firedSizeBlock = numPerBlock;
		} else {
			firedSizeBlock = 0;
		}

		for (int idx = 0; idx < firedSizeBlock; idx++) {
			int nid = firedTable[timeIdx*gFiredTableCap + (block_idx)*numPerBlock + idx];
			int startLoc = connection->pDelayStart[delta_t + nid * delayLength];
			int synapseNum = connection->pDelayNum[delta_t + nid * delayLength];
			if (threadIdx.x == 0) {
				gLayerInput[nid]++;
			}
			for (int j=threadIdx.x; j<synapseNum; j += blockDim.x) {
				//int sid = connection->pSynapsesIdx[j+startLoc];
				int sid = j+startLoc;
				real weight = data->pWeight[sid];
				if (weight >= 0) {
					atomicAdd(&(currentE[data->pDst[sid]]), weight);
				} else {
					atomicAdd(&(currentI[data->pDst[sid]]), weight);
				}

				data->pAPre[sid] *= exp((data->pLastUpdate[sid] - time) / (data->pTauPre[sid]));
				data->pAPost[sid] *= exp((data->pLastUpdate[sid] - time) / (data->pTauPost[sid]));

				data->pAPre[sid] += data->pDPre[sid];
				data->pWeight[sid] = _clip(weight + data->pAPost[sid], G_MIN, G_MAX);
				data->pLastUpdate[sid] = time;
			}
		}
		__syncthreads();
	}
}

__global__ void learn_stdp_synapse_post(Connection *connection, STDPData * data, int *firedTable, int *firedTableSizes, int num, int start_id, int time)
{
	int block_idx = blockIdx.x;
	int timeIdx = time%(connection->maxDelay+1);
	int firedSize = firedTableSizes[timeIdx];
	int numPerBlock = (firedSize - 1) / gridDim.x + 1;
	int blockNumMinusOne = (firedSize - 1) / numPerBlock;
	int firedSizeBlock = 0;
	if (block_idx == blockNumMinusOne) {
		firedSizeBlock = firedSize - block_idx * numPerBlock;
	} else if (block_idx < blockNumMinusOne) {
		firedSizeBlock = numPerBlock;
	} else {
		firedSizeBlock = 0;
	}

	for (int idx = 0; idx < firedSizeBlock; idx++) {
		int nid = firedTable[timeIdx * gFiredTableCap + (block_idx) * numPerBlock + idx];
		int startLoc = connection->pDelayStartRev[nid];
		int synapseNum = connection->pDelayNumRev[nid];
		for (int j=threadIdx.x; j<synapseNum; j+=blockDim.x) {
			int sid = connection->pSidMapRev[j+startLoc];
			data->pAPre[sid] *= exp((data->pLastUpdate[sid] - time) / (data->pTauPre[sid]));
			data->pAPost[sid] *= exp((data->pLastUpdate[sid] - time) / (data->pTauPost[sid]));
			data->pAPost[sid] += data->pDPost[sid];
			data->pWeight[sid] = _clip(data->pWeight[sid] + data->pAPre[sid], G_MIN, G_MAX);
			data->pLastUpdate[sid] = time;
		}
	}
	__syncthreads();
}

void cudaUpdateSTDP(Connection * connection, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time, BlockSize *pSize)
{
	//update_stdp_hit<<<pSize->gridSize, pSize->blockSize>>>((STDPData*)data, num, start_id);
	//reset_active_synapse<<<1, 1>>>();
	update_stdp_hit<<<pSize->gridSize, pSize->blockSize>>>((Connection *)connection,  (STDPData *)data, currentE, currentI, firedTable, firedTableSizes, num, start_id, time);
	learn_stdp_synapse_post<<<pSize->gridSize, pSize->blockSize>>>((Connection *)connection,  (STDPData *)data, firedTable, firedTableSizes, num, start_id, time);

}

