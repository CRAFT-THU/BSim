
#include "runtime.h"
#include "LIF_curr_exp.h"


void update_lif_curr_exp(Lif_curr_exp *data, int num, int start_id, int t)
{
	find_lif_curr_exp_gpu<<<LIF_CURR_EXP_GRID_SIZE, LIF_CURR_EXP_BLOCK_SIZE>>>((Lif_curr_exp*)data, num, start_id);
	update_lif_curr_exp_gpu<<<LIF_CURR_EXP_GRID_SIZE, LIF_CURR_EXP_BLOCK_SIZE>>>((Lif_curr_exp*)data, num, start_id);
}

__global void find_lif_curr_exp_gpu(LIF_curr_exp *data, int num, int start_id)
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
		bool actived = data->p_refrac_step[idx] <= 0;
		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		} else {
			gNeuronInput[start_id + idx] = 0;
			gNeuronInput_I[start_id + idx] = 0;
			data->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;
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

__global void update_lif_curr_exp_gpu(LIF_curr_exp *data, int num, int start_id)
{
	for (int idx = tid; idx < gActiveTableSize; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;
		int nid = gActiveTable[idx];
		int gnid = start_id + nid; 

		data->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];
		data->p_i_E[nid] *= data->p_CE[nid];
		data->p_i_I[nid] *= data->p_CI[nid];

		fired = data->p_vm[nid] >= data->p_v_thresh[nid];
		gFireCount[gnid] += fired;
		if (fired) {
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				fire_table_t[test_loc] = gnid;
				fired = false;
			}
			data->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;
			data->p_vm[nid] = data->p_v_reset[nid];
		} else {
			gXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];
			data->p_i_E[nid] += gNeuronInput[gnid];
			data->p_i_I[nid] += gNeuronInput_I[gnid];
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

