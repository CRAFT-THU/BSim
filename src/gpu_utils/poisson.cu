
#include "poisson.h"

__device__ void reset_poisson_neuron(GPoissonNeurons *d_neurons, int *shared_buf, volatile int size, int start_id) 
{
	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		int nid = shared_buf[idx] - start_id;

		curandState localState = d_neurons->p_state[nid];
		int tmp = curand_poisson(&localState, d_neurons->p_rate[nid]);
		d_neurons->p_fire_cycle[nid] = d_neurons->p_fire_cycle[nid] + 1 + tmp + d_neurons->p_refrac_step[nid];
		d_neurons->p_state[nid] = localState;
	}
}

__global__ void update_poisson_neuron(GPoissonNeurons *d_neurons, int num, int start_id)
{
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

		fired = (gCurrentCycle == d_neurons->p_fire_cycle[idx]) && (gCurrentCycle <= d_neurons->p_end_cycle[idx]);
		gFireCount[start_id + idx] += fired;

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAXBLOCKSIZE) {
					fire_table_t[test_loc] = start_id + idx;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAXBLOCKSIZE) {
				commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
				reset_poisson_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
	}
	__syncthreads();

	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
		reset_poisson_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}
