
#include "../../gpu_utils/runtime.h"

// #include "GConstantNeurons.h"
#include "constants.h"


__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id, int time)
{
	int currentIdx = time % (MAX_DELAY+1);
	__shared__ int fire_table_t[MAXBLOCKSIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		fired = (time < d_neurons->p_end_cycle[idx]) && (((time - d_neurons->p_start_cycle[idx]) * d_neurons->p_fire_rate[idx]) > (d_neurons->p_fire_count[idx]));
		gFireCount[start_id + idx] += fired;

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAXBLOCKSIZE) {
					fire_table_t[test_loc] = start_id + idx;
					d_neurons->p_fire_count[idx] = d_neurons->p_fire_count[idx] + 1;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAXBLOCKSIZE) {
				commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &(gFiredTableSizes[currentIdx]), gFiredTableCap*currentIdx);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
	}
	__syncthreads();

	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &(gFiredTableSizes[currentIdx]), gFiredTableCap*currentIdx);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

int cudaUpdateConstant(void *data, int num, int start_id, int t, BlockSize *pSize)
{
	update_constant_neuron<<<pSize->gridSize, pSize->blockSize>>>((GConstantNeurons*)data, num, start_id, t);

	return 0;
}

