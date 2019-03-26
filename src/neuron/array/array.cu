
#include "../../gpu_utils/runtime.h"

// #include "GArrayNeurons.h"
#include "array.h"


__global__ void update_array_neuron(GArrayNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time)
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

		fired = (data->p_start[idx] < data->p_end[idx]) &&  (time >= data->p_fire_time[data->p_start[idx]]);
		gFireCount[start_id + idx] += fired;

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAX_BLOCK_SIZE) {
					fire_table_t[test_loc] = start_id + idx;
					data->p_start[idx] = data->p_start[idx] + 1;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAX_BLOCK_SIZE) {
				commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &(firedTableSizes[currentIdx]), gFiredTableCap*currentIdx);
				//advance_array_neuron(data, fire_table_t, MAX_BLOCK_SIZE, start_id);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
	}
	__syncthreads();

	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, firedTable, &(firedTableSizes[currentIdx]), gFiredTableCap*currentIdx);
		//advance_array_neuron(data, fire_table_t, fire_cnt, start_id);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

void cudaUpdateArray(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int time, BlockSize *pSize)
{
	update_array_neuron<<<pSize->gridSize, pSize->blockSize>>>((GArrayNeurons*)data, currentE, currentI, firedTable, firedTableSizes, num, start_id, time);
}

