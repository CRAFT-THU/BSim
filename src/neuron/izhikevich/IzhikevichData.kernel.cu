
#include "IzhikevichData.h"

#include "../../gpu_utils/runtime.h"
#include "../../net/Connection.h"


__global__ void update_izhikevich_neuron(Connection *connection, IzhikevichData *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	int currentIdx = time % (connection->maxDelay+1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int fire_cnt;
	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		int testLoc = 0;

		int nid = idx;
		int gnid = offset + nid; 

		real u = data->pU[nid];
		real v = data->pV[nid];
		real a = data->pA[nid];
		real b = data->pB[nid];
		real c = data->pC[nid];
		real d = data->pD[nid];

		const bool oldSpike = (v >= 29.99f);

		if (v >= 30.0f) {
			v = c;
			u += d;
		}

		const real I = currentE[gnid] + currentI[gnid];

		v += 0.5f * (0.04f * v * v + 5.0f * v + 140.0f -u + I) * DT;
		v += 0.5f * (0.04f * v * v + 5.0f * v + 140.0f -u + I) * DT;
	
		u += a*(b*v - u) * DT;

		if (v > 30.0f) {
			v = 30.0f;
		}

		bool fired = (v >= 29.99f) && (!oldSpike);

		data->pU[nid] = u;
		data->pV[nid] = v;
		data->pA[nid] = a;
		data->pB[nid] = b;
		data->pC[nid] = c;
		data->pD[nid] = d;

		if (fired) {
			testLoc = atomicAdd((int*)&fire_cnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				fire_table_t[testLoc] = gnid;
				fired = false;
			}
		} else {
			gXInput[gnid] += I;
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

void cudaUpdateIzhikevich(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time, BlockSize *pSize)
{
	update_izhikevich_neuron<<<pSize->gridSize, pSize->blockSize>>>(conn, (IzhikevichData *)data, currentE, currentI, firedTable, firedTableSizes, num, offset, time);
}
