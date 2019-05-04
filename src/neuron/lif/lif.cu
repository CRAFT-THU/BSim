
#include "GLIFNeurons.h"

#include "../../gpu_utils/runtime.h"
#include "../../net/Connection.h"

// #include "GLIFNeurons.h"
#include "lif.h"


__global__ void find_lif_neuron(GLIFNeurons *data, real * currentE, real * currentI, int num, int offset)
{
	__shared__ int tActiveTable[MAX_BLOCK_SIZE];
	__shared__ volatile int activeCnt;

	if (threadIdx.x == 0) {
		activeCnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		//bool actived = false;
		int testLoc = 0;
		bool actived = data->pRefracStep[idx] <= 0;
		if (actived) {
			testLoc = atomicAdd((int*)&activeCnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				tActiveTable[testLoc] = idx;
				actived = false;
			}
		} else {
			currentE[offset + idx] = 0;
			currentI[offset + idx] = 0;
			data->pRefracStep[idx] = data->pRefracStep[idx] - 1;
		}
		__syncthreads();

		if (activeCnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(tActiveTable, MAX_BLOCK_SIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				activeCnt = 0;
			}
		}
		__syncthreads();

		if (actived) {
			testLoc = atomicAdd((int*)&activeCnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				tActiveTable[testLoc] = idx;
				actived = false;
			}
		}
		__syncthreads();

		if (activeCnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(tActiveTable, MAX_BLOCK_SIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				activeCnt = 0;
			}
		}
		__syncthreads();

		if (activeCnt > 0) {
			commit2globalTable(tActiveTable, activeCnt, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				activeCnt = 0;
			}
		}
		__syncthreads();
	}
}

__global__ void update_lif_neuron(Connection *connection, GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	int currentIdx = time % (connection->maxDelay-connection->minDelay+1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int fire_cnt;
	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int testLoc = 0;

		int nid = gActiveTable[idx];
		int gnid = offset + nid; 

		//real I = sqrtf(data->pCe[nid]) * data->pI_e[nid] + sqrtf(data->pCi[nid]) * data->pI_i[nid] + data->p_i_tmp[nid];

		//real I = currentE[gnid] + data->p_i_tmp[nid];
		//data->pV_m[nid] = data->pV_m[nid] * data->p_C1[nid] + data->p_C2[nid] * I;

		data->pV_m[nid] = data->pC_m[nid] * data->pV_m[nid] + data->pV_tmp[nid] + data->pI_e[nid] * data->pC_e[nid] + data->pI_i[nid] * data->pC_i[nid];

		//data->p_i_syn[nid] = 0;

		data->pI_e[nid] *= data->pCe[nid];
		data->pI_i[nid] *= data->pCi[nid];

		fired = data->pV_m[nid] >= data->pV_thresh[nid];

		gFireCount[gnid] += fired;

		if (fired) {
			testLoc = atomicAdd((int*)&fire_cnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				fire_table_t[testLoc] = gnid;
				fired = false;
			}

			data->pRefracStep[nid] = data->pRefracTime[nid] - 1;
			data->pV_m[nid] = data->pV_reset[nid];
		} else {
			gXInput[gnid] += currentE[gnid] + currentI[gnid];
			data->pI_e[nid] += currentE[gnid];
			data->pI_i[nid] += currentI[gnid];
		}

		currentE[gnid] = 0;
		currentI[gnid] = 0;

		__syncthreads();
		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}

		__syncthreads();

		if (fired) {
			testLoc = atomicAdd((int*)&fire_cnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				fire_table_t[testLoc] = gnid;
				fired = false;
			}
		}
		__syncthreads();
		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

		if (fire_cnt > 0) {
			commit2globalTable(fire_table_t, fire_cnt, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}

	}
	//__syncthreads();
	//if (threadIdx.x == 0 && blockIdx.x == 0) {
	//	gActiveTableSize = 0;
	//}
}

__global__ void update_all_lif_neuron(Connection *connection, GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
// __global__ void update_all_lif_neuron(GLIFNeurons *data, int num, int offset, int time)
{
	int currentIdx = time % (connection->maxDelay - connection->minDelay + 1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}

	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int testLoc = 0;

		int nid = idx;
		int gnid = offset + idx; 
		bool actived = data->pRefracStep[idx] <= 0;

		if (actived) {
			//real I = sqrtf(data->pCe[nid]) * data->pI_e[nid] + sqrtf(data->pCi[nid]) * data->pI_i[nid] + data->p_i_tmp[nid];

			//real I = currentE[gnid] + data->p_i_tmp[nid];
			//data->pV_m[nid] = data->pV_m[nid] * data->p_C1[nid] + data->p_C2[nid] * I;
			//data->p_i_syn[nid] = 0;

			data->pV_m[nid] = data->pC_m[nid] * data->pV_m[nid] + data->pV_tmp[nid] + data->pI_e[nid] * data->pC_e[nid] + data->pI_i[nid] * data->pC_i[nid];

			gXInput[gnid] += currentE[gnid] + currentI[gnid];

			data->pI_e[nid] *= data->pCe[nid];
			data->pI_i[nid] *= data->pCi[nid];

			fired = data->pV_m[nid] >= data->pV_thresh[nid];

			gFireCount[gnid] += fired;

			if (fired) {
				testLoc = atomicAdd((int*)&fire_cnt, 1);
				if (testLoc < MAX_BLOCK_SIZE) {
					fire_table_t[testLoc] = gnid;
					fired = false;
				}

				data->pRefracStep[nid] = data->pRefracTime[nid] - 1;
				data->pV_m[nid] = data->pV_reset[nid];
			} else {
				data->pI_e[nid] += currentE[gnid];
				data->pI_i[nid] += currentI[gnid];
			}

			__syncthreads();
			if (fire_cnt >= MAX_BLOCK_SIZE) {
				commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}

			__syncthreads();

			if (fired) {
				testLoc = atomicAdd((int*)&fire_cnt, 1);
				if (testLoc < MAX_BLOCK_SIZE) {
					fire_table_t[testLoc] = gnid;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAX_BLOCK_SIZE) {
				commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();

			if (fire_cnt > 0) {
				commit2globalTable(fire_table_t, fire_cnt, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
		} else {
			data->pRefracStep[idx] = data->pRefracStep[idx] - 1;
		}
		currentE[gnid] = 0;
		currentI[gnid] = 0;
	}
	__syncthreads();
}

__global__ void update_dense_lif_neuron(Connection *connection, GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	//__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	//__shared__ volatile int fire_cnt;

	//if (threadIdx.x == 0) {
	//	fire_cnt = 0;
	//}
	//__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int currentIdx = time % (connection->maxDelay-connection->minDelay+1);
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		//bool fired = false;
		//int testLoc = 0;

		int nid = idx;
		int gnid = offset + idx; 
		bool actived = data->pRefracStep[idx] <= 0;

		if (actived) {
			data->pV_m[nid] = data->pC_m[nid] * data->pV_m[nid] + data->pV_tmp[nid] + data->pI_e[nid] * data->pC_e[nid] + data->pI_i[nid] * data->pC_i[nid];

			data->pI_e[nid] *= data->pCe[nid];
			data->pI_i[nid] *= data->pCi[nid];

			bool fired = data->pV_m[nid] >= data->pV_thresh[nid];

			firedTable[gFiredTableCap*currentIdx + gnid] = fired;

			gFireCount[gnid] += fired;

			if (fired) {
				data->pRefracStep[nid] = data->pRefracTime[nid] - 1;
				data->pV_m[nid] = data->pV_reset[nid];

			} else {
				gXInput[gnid] += currentE[gnid] + currentI[gnid];
				data->pI_e[nid] += currentE[gnid];
				data->pI_i[nid] += currentI[gnid];
				//real input = 0, input_I = 0;
				//for (int i=data->p_start_E[nid]; i<data->p_start_I[nid]; i++) {
				//	input += currentE[i];
				//}
				//for (int i=data->p_start_I[nid]; i<data->p_end[nid]; i++) {
				//	input_I += currentE[i];
				//}
				//data->pI_e[nid] += input;
				//data->pI_i[nid] += input_I;
				//gXInput[gnid] += input + input_I;
			}

		} else {
			data->pRefracStep[idx] = data->pRefracStep[idx] - 1;
			firedTable[gFiredTableCap*currentIdx + gnid] = 0;
		}
		currentE[gnid] = 0;
		currentI[gnid] = 0;
	}
	__syncthreads();
}

void cudaUpdateLIF(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time, BlockSize *pSize)
{
	find_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, currentE, currentI, num, offset);
	update_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>(conn, (GLIFNeurons*)data, currentE, currentI, firedTable, firedTableSizes, num, offset, time);
	//update_dense_life_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, offset);

}
