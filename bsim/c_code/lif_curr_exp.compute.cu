
#include "runtime.h"
#include "lif_curr_exp.h"


__global__ void find_lif_curr_exp_gpu(Lif_curr_exp *data, int num, int start_id)
{
	__shared__ int active_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int active_cnt;

	if (threadIdx.x == 0 && blockIdx.x == 0) {
		active_cnt = 0;
		g_active_lif_curr_exp_table_size = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		int test_loc = 0;
		bool actived = data->p_refract_step[idx] <= 0;
		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		} else {
			g_i_exec[start_id + idx] = 0;
			g_i_inh[start_id + idx] = 0;
			data->p_refract_time[idx] = data->p_refract_time[idx] - 1;
		}
		__syncthreads();

		if (active_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(active_table_t, MAX_BLOCK_SIZE, g_active_lif_curr_exp_table, &g_active_lif_curr_exp_table_size, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();

		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		}
		__syncthreads();

		if (active_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(active_table_t, MAX_BLOCK_SIZE, g_active_lif_curr_exp_table, &g_active_lif_curr_exp_table_size, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();

		if (active_cnt > 0) {
			commit2globalTable(active_table_t, MAX_BLOCK_SIZE, g_active_lif_curr_exp_table, &g_active_lif_curr_exp_table_size, 0);
			if (threadIdx.x == 0) {
				active_cnt = 0;
			}
		}
		__syncthreads();
	}
}

__global__ void update_lif_curr_exp_gpu(Lif_curr_exp *data, int num, int start_id, int t)
{
	int delay_idx = t % (MAX_DELAY+1)
	if (threadIdx.x == 0 && blockIdx.x==0) {
		g_fired_table_sizes[delay_idx] = 0;
	}
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < g_active_lif_curr_exp_table_size; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;
		int nid = g_active_lif_curr_exptable[idx];
		int gnid = start_id + nid; 

		data->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];
		data->p_i_E[nid] *= data->p_CE[nid];
		data->p_i_I[nid] *= data->p_CI[nid];

		fired = data->p_vm[nid] >= data->p_v_thresh[nid];
		gFireCount[gnid] += fired;
		if (fired) {
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				fire_table_t[test_loc] = gnid;
				fired = false;
			}
			data->p_refract_time[nid] = data->p_refrac_time[nid] - 1;
			data->p_vm[nid] = data->p_v_reset[nid];
		} else {
			data->p_i_E[nid] += g_i_exec[gnid];
			data->p_i_I[nid] += g_i_inh[gnid];
		}

		g_i_exec[gnid] = 0;
		g_i_inh[gnid] = 0;
		__syncthreads();

		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, g_fired_table, &g_fired_table_sizes[delay_idx], g_fired_tableCap*delay_idx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

		if (fired) {
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				fire_table_t[test_loc] = gnid;
				fired = false;
			}
		}
		__syncthreads();
		if (fire_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, g_fired_table, &g_fired_tableSizes[delay_idx], g_fired_tableCap*delay_idx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();
		
		if (fire_cnt > 0) {
			commit2globalTable(fire_table_t, fire_cnt, g_fired_table, &g_fired_tableSizes[delay_idx], g_fired_tableCap*delay_idx);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		
	}
}

void update_lif_curr_exp(Lif_curr_exp *data, int num, int start_id, int t)
{
	find_lif_curr_exp_gpu<<<LIF_CURR_EXP_GRID_SIZE, LIF_CURR_EXP_BLOCK_SIZE>>>((Lif_curr_exp*)data, num, start_id);
	update_lif_curr_exp_gpu<<<LIF_CURR_EXP_GRID_SIZE, LIF_CURR_EXP_BLOCK_SIZE>>>((Lif_curr_exp*)data, num, start_id);
}

