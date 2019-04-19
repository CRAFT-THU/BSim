
#include "GLIFNeurons.h"

#include "../../gpu_utils/runtime.h"

// #include "GLIFNeurons.h"
#include "lif.h"


__global__ void find_lif_neuron(GLIFNeurons *data, real * currentE, real * currentI, int num, int offset)
{
	__shared__ int active_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int active_cnt;

	if (threadIdx.x == 0) {
		active_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		//bool actived = false;
		int test_loc = 0;
		bool actived = data->p_refrac_step[idx] <= 0;
		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		} else {
			currentE[offset + idx] = 0;
			currentI[offset + idx] = 0;
			data->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;
		}
		__syncthreads();

		if (active_cnt >= MAX_BLOCK_SIZE) {
			commit2globalTable(active_table_t, MAX_BLOCK_SIZE, gActiveTable, &gActiveTableSize, 0);
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
			commit2globalTable(active_table_t, MAX_BLOCK_SIZE, gActiveTable, &gActiveTableSize, 0);
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

__global__ void update_lif_neuron(GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	int currentIdx = time % (MAX_DELAY+1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
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
		int gnid = offset + nid; 

		//real I = sqrtf(data->p_CE[nid]) * data->p_i_E[nid] + sqrtf(data->p_CI[nid]) * data->p_i_I[nid] + data->p_i_tmp[nid];

		//real I = currentE[gnid] + data->p_i_tmp[nid];
		//data->p_vm[nid] = data->p_vm[nid] * data->p_C1[nid] + data->p_C2[nid] * I;

		data->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];

		//data->p_i_syn[nid] = 0;

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

			data->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;
			data->p_vm[nid] = data->p_v_reset[nid];
		} else {
			gXInput[gnid] += currentE[gnid] + currentI[gnid];
			data->p_i_E[nid] += currentE[gnid];
			data->p_i_I[nid] += currentI[gnid];
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
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < MAX_BLOCK_SIZE) {
				fire_table_t[test_loc] = gnid;
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

__global__ void update_all_lif_neuron(GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
// __global__ void update_all_lif_neuron(GLIFNeurons *data, int num, int offset, int time)
{
	int currentIdx = time % (MAX_DELAY+1);
	__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	__shared__ volatile int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}

	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		int nid = idx;
		int gnid = offset + idx; 
		bool actived = data->p_refrac_step[idx] <= 0;

		if (actived) {
			//real I = sqrtf(data->p_CE[nid]) * data->p_i_E[nid] + sqrtf(data->p_CI[nid]) * data->p_i_I[nid] + data->p_i_tmp[nid];

			//real I = currentE[gnid] + data->p_i_tmp[nid];
			//data->p_vm[nid] = data->p_vm[nid] * data->p_C1[nid] + data->p_C2[nid] * I;
			//data->p_i_syn[nid] = 0;

			data->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];

			gXInput[gnid] += currentE[gnid] + currentI[gnid];

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

				data->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;
				data->p_vm[nid] = data->p_v_reset[nid];
			} else {
				data->p_i_E[nid] += currentE[gnid];
				data->p_i_I[nid] += currentI[gnid];
			}

			__syncthreads();
			if (fire_cnt >= MAX_BLOCK_SIZE) {
				commit2globalTable(fire_table_t, MAX_BLOCK_SIZE, firedTable, &firedTableSizes[currentIdx], gFiredTableCap*currentIdx);
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
		} else {
			data->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;
		}
		currentE[gnid] = 0;
		currentI[gnid] = 0;
	}
	__syncthreads();
}

__global__ void update_dense_lif_neuron(GLIFNeurons *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	//__shared__ int fire_table_t[MAX_BLOCK_SIZE];
	//__shared__ volatile int fire_cnt;

	//if (threadIdx.x == 0) {
	//	fire_cnt = 0;
	//}
	//__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int currentIdx = time % (MAX_DELAY+1);
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		//bool fired = false;
		//int test_loc = 0;

		int nid = idx;
		int gnid = offset + idx; 
		bool actived = data->p_refrac_step[idx] <= 0;

		if (actived) {
			data->p_vm[nid] = data->p_Cm[nid] * data->p_vm[nid] + data->p_v_tmp[nid] + data->p_i_E[nid] * data->p_C_E[nid] + data->p_i_I[nid] * data->p_C_I[nid];

			data->p_i_E[nid] *= data->p_CE[nid];
			data->p_i_I[nid] *= data->p_CI[nid];

			bool fired = data->p_vm[nid] >= data->p_v_thresh[nid];

			firedTable[gFiredTableCap*currentIdx + gnid] = fired;

			gFireCount[gnid] += fired;

			if (fired) {
				data->p_refrac_step[nid] = data->p_refrac_time[nid] - 1;
				data->p_vm[nid] = data->p_v_reset[nid];

			} else {
				gXInput[gnid] += currentE[gnid] + currentI[gnid];
				data->p_i_E[nid] += currentE[gnid];
				data->p_i_I[nid] += currentI[gnid];
				//real input = 0, input_I = 0;
				//for (int i=data->p_start_E[nid]; i<data->p_start_I[nid]; i++) {
				//	input += currentE[i];
				//}
				//for (int i=data->p_start_I[nid]; i<data->p_end[nid]; i++) {
				//	input_I += currentE[i];
				//}
				//data->p_i_E[nid] += input;
				//data->p_i_I[nid] += input_I;
				//gXInput[gnid] += input + input_I;
			}

		} else {
			data->p_refrac_step[idx] = data->p_refrac_step[idx] - 1;
			firedTable[gFiredTableCap*currentIdx + gnid] = 0;
		}
		currentE[gnid] = 0;
		currentI[gnid] = 0;
	}
	__syncthreads();
}

void cudaUpdateLIF(void *data, void * unUsed, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time, BlockSize *pSize)
{
	find_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, currentE, currentI, num, offset);
	update_lif_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, currentE, currentI, firedTable, firedTableSizes, num, offset, time);
	//update_dense_life_neuron<<<pSize->gridSize, pSize->blockSize>>>((GLIFNeurons*)data, num, offset);

}
