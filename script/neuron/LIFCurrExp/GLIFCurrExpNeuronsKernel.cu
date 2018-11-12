
#include "../../gpu_utils/runtime.h"

#include "GLIFCurrExpNeuronsKernel.h"

__global void find_lifcurrexp_neurons(GLIFCurrExpNeurons *d_neurons, int num, int start_id)
{
	__shared__ int active_table_t[MAXBLOCKSIZE];
	__shared__ volatile int active_cnt;

	if (threadIdx.x == 0) {
		active_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		int test_loc = 0;
		
		bool actived = d_neurons->p_refrac_step[idx] <= 0;
		
		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		} else {
			gNeuronInput[start_id + idx] = 0;
			gNeuronInput_I[start_id + idx] = 0;
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
		}
		__syncthreads();
		
		if (active_cnt >= MAXBLOCKSIZE) {
			commit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();
		
		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		}
		__syncthreads();
		if (active_cnt >= MAXBLOCKSIZE) {
			commit2globalTable(active_table_t, MAXBLOCKSIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();
		if (active_cnt > 0) {
			commit2globalTable(active_table_t, active_cnt, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();
	}
}


__global void update_lifcurrexp_neurons(GLIFCurrExpNeurons *d_neurons, int num, int start_id)
{
	__shared__ int fire_table_t[MAXBLOCKSIZE];
	__shared__ volatile int fire_cnt;
	
	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();
	
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;
		
		int nid = gActiveTable[idx];
		int gnid = start_id + nid; 
		
		d_neurons->p_vm[nid] = d_neurons->p_Cm[nid] * d_neurons->p_vm[nid] + d_neurons->p_v_tmp[nid] + d_neurons->p_i_E[nid] * d_neurons->p_C_E[nid] + d_neurons->p_i_I[nid] * d_neurons->p_C_I[nid];
		
		d_neurons->p_i_E[nid] *= d_neurons->p_CE[nid];
		d_neurons->p_i_I[nid] *= d_neurons->p_CI[nid];
		
		fired = d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid];
		
		gFireCount[gnid] += fired;
		
		if (fired) {
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				fire_table_t[test_loc] = gnid;
				fired = false;
			}
			
			d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
			d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
		} else {
			gXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];
			d_neurons->p_i_E[nid] += gNeuronInput[gnid];
			d_neurons->p_i_I[nid] += gNeuronInput_I[gnid];
		}
		
		gNeuronInput[gnid] = 0;
		gNeuronInput_I[gnid] = 0;
		__syncthreads();
		
		if (fire_cnt >= MAXBLOCKSIZE) {
			commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();
		
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
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();
		
		if (fire_cnt > 0) {
			commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		
	}
}
