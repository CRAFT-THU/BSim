
#include "../../gpu_utils/runtime.h"
#include "../../net/Connection.h"

#include "TraubMilesData.h"

__global__ void update_traubmiles(Connection *connection, TraubMilesData *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	int currentIdx = time % (connection->maxDelay+1);
	__shared__ int fireTableT[MAX_BLOCK_SIZE];
	__shared__ volatile int fireCNT;
	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
		int nid = idx;
		int gnid = offset + nid;
		int testLoc = 0;
		bool fired = false;

		real gNa = data->pGNa[nid];
		real ENa = data->pENa[nid];
		real gK = data->pGK[nid];
		real EK = data->pEK[nid];
		real gl = data->pGl[nid];
		real El = data->pEl[nid];
		real C = data->pC[nid];
		real v = data->pV[nid];
		real m = data->pM[nid];
		real h = data->pH[nid];
		real n = data->pN[nid];
		real decayE = data->pDecayE[nid];
		real decayI = data->pDecayI[nid];
		real EE = data->pEE[nid];
		real EI = data->pEI[nid];

		real mDt = DT/25.0;
		
		real i = currentE[gnid] * (EE-v);
		i += currentI[gnid] * (EI-v);

		const bool oldSpike = (v >= 0);

		for (int i=0; i<25; i++) {
           real iTmp = -(m*m*m*h*gNa*(V-ENa) + n*n*n*n*gK*(V-EK) + gl*(V-El)- i);
           real a = (v==-52.0)?1.28:(0.32*(-52.0-V)/(exp((-52.0-V)/4.0)-1.0));
           real b = (v==-25.0)?1.4:(0.28*(v+25.0)/(exp((v+25.0)/5.0)-1.0));

           m += (a*(1.0-m)-b*m)*mDt;
           a= 0.128*exp((-48.0-v)/18.0);
           b= 4.0 / (exp((-25.0-v)/5.0)+1.0);
           h += (a*(1.0-h)-b*h)*mDt;
		   a = (v == -50.0)?0.16:(0.032*(-50.0-v)/(exp((-50.0-v)/5.0)-1.0));
           b = 0.5*exp((-55.0-v)/40.0);
           n += (a*(1.0-n)-b*n)*mDt;
           v += iTmp/C*mDt;
		}

		// data->pGNa[nid] = gNa;
		// data->pENa[nid] = ENa;
		// data->pGK[nid] = gK;
		// data->pEK[nid] = EK;
		// data->pGl[nid] = gl;
		// data->pEl[nid] = El;
		// data->pC[nid] = C;
		data->pV[nid] = v;
		data->pM[nid] = m;
		data->pH[nid] = h;
		data->pN[nid] = n;
		data->pDecay[nid] = decay;
		data->pE[nid] = E;

		if (fired) {
			testLoc = atomicAdd((int*)&fire_cnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				fireTableT[testLoc] = gnid;
				fired = false;
			}
		}
		__syncthreads();

		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fireTableT, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

		if (fired) {
			testLoc = atomicAdd((int*)&fire_cnt, 1);
			if (testLoc < MAX_BLOCK_SIZE) {
				fireTableT[testLoc] = gnid;
				fired = false;
			}
		}
		__syncthreads();

		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fireTableT, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

		if (fire_cnt > 0) {
			commit2globalTable(fireTableT, fire_cnt, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

	}
}


void cudaUpdateTraubMiles(Connection *connection, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time, BlockSize *pSize)
{
	update_traubmiles<<<pSize->gridSize, pSize->blockSize>>>(connection, (TraubMilesData *)data, currentE, currentI, firedTable, firedTableSizes, num, offset, time);
}


