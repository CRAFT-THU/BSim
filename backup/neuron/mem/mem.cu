
#include "../../gpu_utils/runtime.h"

// #include "GMemNeurons.h"
#include "mem.h"


__global__ void update_mem_neuron(GMemNeurons *d_neurons, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time)
{
	int currentIdx = time % (MAX_DELAY+1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;
		int gnid = idx + start_id;


		//fired = (time * d_neurons->p_fire_rate[idx]) > (d_neurons->p_fire_count[idx]);
		d_neurons->p_fire_rate[idx] = d_neurons->p_fire_rate[idx] + currentE[gnid] + currentI[gnid];
		fired = d_neurons->p_fire_rate[idx] > d_neurons->p_fire_count[idx];
		gFireCount[gnid] += fired;

		//d_neurons->p_fire_rate[idx] = (d_neurons->p_fire_rate[idx] * (time) + currentE[gnid] + currentI[gnid])/(time + 1);

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAX_BLOCK_SIZE) {
					fire_table_t[test_loc] = start_id + idx;
					d_neurons->p_fire_count[idx] = d_neurons->p_fire_count[idx] + 1;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAX_BLOCK_SIZE) {
				commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &(firedTableSizes[currentIdx]), gFiredTableCap*currentIdx);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}

		gXInput[gnid] += currentE[gnid] + currentI[gnid];

		currentE[gnid] = 0;
		currentI[gnid] = 0;
		__syncthreads();
	}
	__syncthreads();

	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, firedTable, &(firedTableSizes[currentIdx]), gFiredTableCap*currentIdx);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

void cudaUpdateMem(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time, BlockSize *pSize)
{
	update_mem_neuron<<<pSize->gridSize, pSize->blockSize>>>((GMemNeurons*)data, currentE, currentI, firedTable, firedTableSizes, num, start_id, time);
}

