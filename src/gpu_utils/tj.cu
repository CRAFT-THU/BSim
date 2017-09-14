
#include "tj.h"

__device__ void reset_tj_neuron(GTJNeurons *d_neurons, int *shared_buf, volatile int size, int start_id) 
{
	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		int nid = shared_buf[idx] - start_id;
		d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
		d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
	}
}

__global__ void update_tj_neuron(GTJNeurons *d_neurons, int num, int start_id)
{
	__shared__ int fire_table_t[MAXBLOCKSIZE];
	__shared__ volatile int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		int gnid = start_id + idx; 
		bool actived = d_neurons->p_refrac_step[idx] <= 0;
		//real DT = 0.001;
		if (actived) {
			real I = gNeuronInput[gnid] + gNeuronInput_I[gnid] + d_neurons->p_i_tmp[idx];
			d_neurons->p_vm[idx] = d_neurons->p_vm[idx] + DT * I/d_neurons->p_cm[idx];
			gXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];

			fired = d_neurons->p_vm[idx] >= d_neurons->p_v_thresh[idx];

			gFireCount[gnid] += fired;

			for (int i=0; i<2; i++) {
				if (fired) {
					test_loc = atomicAdd((int*)&fire_cnt, 1);
					if (test_loc < MAXBLOCKSIZE) {
						fire_table_t[test_loc] = gnid;
						fired = false;
					}
				}
				__syncthreads();
				if (fire_cnt >= MAXBLOCKSIZE) {
					commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
					reset_tj_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
					if (threadIdx.x == 0) {
						fire_cnt = 0;
					}
				}
				__syncthreads();
			}
			__syncthreads();

			if (fire_cnt > 0) {
				commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
				reset_tj_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
		} else {
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
		}
		gNeuronInput[start_id + idx] = 0;
		gNeuronInput_I[start_id + idx] = 0;
	}
	__syncthreads();
}

