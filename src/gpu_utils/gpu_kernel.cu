
#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "mem_op.h"
#include "gpu_macros.h"
#include "gpu_kernel.h"

#define MAXBLOCKSIZE 1024

// Constant
__constant__ int MAX_DELAY;
__constant__ int gTimeTableCap;
__constant__ int gFiredTableCap;
__constant__ int gSynapsesTableCap;

// Variable
__device__ int gCurrentIdx;
__device__ int gCurrentCycle;
__device__ int gFiredTableSize;
__device__ int gActiveTableSize;
__device__ int gSynapsesActiveTableSize;

// Arrays
//__device__ int *gTimeTable;

// Neuron Arrays
__device__ real *gNeuronInput;
__device__ real *gNeuronInput_I;

// Neuron Tables
__device__ int *gFiredTable;
__device__ int *gFiredTableSizes;
__device__ int *gActiveTable;

// Synapse Tables
//__device__ int *gSynapsesActiveTable;
//__device__ int *gSynapsesLogTable;

// Log Arrays
__device__ int *gLayerInput;
__device__ real *gXInput;
__device__ int *gFireCount;

// Connection
__device__ N2SConnection *gConnection;

//#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 600
//#else
//__device__ double atomicAdd(double* address, double val)
//{
//	unsigned long long int* address_as_ull = (unsigned long long int*)address;
//	unsigned long long int old = *address_as_ull, assumed;
//	do {
//		assumed = old;
//		old = atomicCAS(address_as_ull, assumed,
//				__double_as_longlong(val + __longlong_as_double(assumed)));
//	} while (assumed != old);
//	return __longlong_as_double(old);
//}
//#endif


__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, int *global_buf, int * global_size, int offset) 
{
	__shared__ volatile unsigned int start_loc;
	if (threadIdx.x == 0) {
		start_loc = atomicAdd(global_size, (int)size);
	}
	__syncthreads();

	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		global_buf[offset + start_loc + idx] = shared_buf[idx];
	}

	return 0;
}

__global__ void update_time()
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		//gTimeTable[gCurrentIdx] = simTime;
		gCurrentCycle = gCurrentCycle + 1;
		gCurrentIdx = (gCurrentIdx +1)%(MAX_DELAY + 1);
		gActiveTableSize = 0;
		gFiredTableSizes[gCurrentIdx] = 0;
		gSynapsesActiveTableSize = 0;
	}
	__syncthreads();
}

__global__ void reset_active_synapse()
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gSynapsesActiveTableSize = 0;
	}
	__syncthreads();

}

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id)
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

		fired = (gCurrentCycle * d_neurons->p_fire_rate[idx]) > (d_neurons->p_fire_count[idx]);

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
				commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
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
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

__global__ void curand_setup_kernel(curandState *state, int num)
{
	int id = threadIdx.x + blockIdx.x * blockDim.x;
	if (id < num) {
		curand_init(1234, id, 0, &state[id]); 
	}
}

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

//__device__ void advance_array_neuron(GArrayNeurons *d_neurons, int *shared_buf, volatile int size, int start_id) 
//{
//	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
//		int offset = shared_buf[idx] - start_id;
//		d_neurons->p_start[offset] = d_neurons->p_start[offset] + 1;
//	}
//}

__global__ void update_array_neuron(GArrayNeurons *d_neurons, int num, int start_id)
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

		fired = (d_neurons->p_start[idx] < d_neurons->p_end[idx]) &&  (gCurrentCycle >= d_neurons->p_fire_time[d_neurons->p_start[idx]]);

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAXBLOCKSIZE) {
					fire_table_t[test_loc] = start_id + idx;
					d_neurons->p_start[idx] = d_neurons->p_start[idx] + 1;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAXBLOCKSIZE) {
				commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
				//advance_array_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
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
		//advance_array_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

__global__ void update_max_neuron(GMaxNeurons *d_neurons, int num, int start_id)
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

		int input = (int)gNeuronInput[start_id + idx];
		gNeuronInput[start_id + idx] = 0;
		int test = 1;
		int record_offset = idx*d_neurons->max_N; 
		for (int i=0; i<d_neurons->p_N[idx]; i++) {
			if (input & test) {
				d_neurons->p_record[record_offset + i]++;
				if (d_neurons->p_record[record_offset + i] > d_neurons->p_count[idx]) {

					fired = true;
				}
			}
			test = test << 1;
		}

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < MAXBLOCKSIZE) {
					fire_table_t[test_loc] = start_id + idx;
					d_neurons->p_count[idx] = d_neurons->p_count[idx] + 1;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= MAXBLOCKSIZE) {
				commit2globalTable(fire_table_t, MAXBLOCKSIZE, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
				//advance_array_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
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
		//advance_array_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}

}

__global__ void find_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id)
{
	__shared__ int active_table_t[MAXBLOCKSIZE];
	__shared__ volatile int active_cnt;

	if (threadIdx.x == 0) {
		active_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool actived = false;
		int test_loc = 0;

		actived = d_neurons->p_refrac_step[idx] <= 0;

		if (actived) {
			test_loc = atomicAdd((int*)&active_cnt, 1);
			if (test_loc < MAXBLOCKSIZE) {
				active_table_t[test_loc] = idx;
				actived = false;
			}
		} else {
			gNeuronInput[start_id + idx] = 0;
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

__device__ void reset_lif_neuron(GLIFNeurons *d_neurons, int *shared_buf, volatile int size, int start_id) 
{
	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		int nid = shared_buf[idx] - start_id;
		d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
		d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
	}
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id)
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
		real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[nid];
		d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
		//d_neurons->p_i_syn[nid] = 0;
		gXInput[gnid] += gNeuronInput[gnid];
		gNeuronInput[gnid] = 0;

		fired = d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid];

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
				reset_lif_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
		__syncthreads();

		if (fire_cnt > 0) {
			commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
			reset_lif_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
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

__global__ void update_all_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id)
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
		if (actived) {
			real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[idx];
			d_neurons->p_vm[idx] = d_neurons->p_vm[idx] * d_neurons->p_C1[idx] + d_neurons->p_C2[idx] * I;
			//d_neurons->p_i_syn[idx] = 0;
			gXInput[gnid] += gNeuronInput[gnid];

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
					reset_lif_neuron(d_neurons, fire_table_t, MAXBLOCKSIZE, start_id);
					if (threadIdx.x == 0) {
						fire_cnt = 0;
					}
				}
				__syncthreads();
			}
			__syncthreads();

			if (fire_cnt > 0) {
				commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
				reset_lif_neuron(d_neurons, fire_table_t, fire_cnt, start_id);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
		} else {
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
		}
		gNeuronInput[start_id + idx] = 0;
	}
	__syncthreads();
}

__global__ void find_life_neuron(GLIFENeurons *d_neurons, int num, int start_id)
{
	__shared__ int active_table_t[MAXBLOCKSIZE];
	__shared__ volatile int active_cnt;

	if (threadIdx.x == 0) {
		active_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		//bool actived = false;
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

__global__ void update_life_neuron(GLIFENeurons *d_neurons, int num, int start_id)
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

		//real I = sqrtf(d_neurons->p_CE[nid]) * d_neurons->p_i_E[nid] + sqrtf(d_neurons->p_CI[nid]) * d_neurons->p_i_I[nid] + d_neurons->p_i_tmp[nid];

		//real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[nid];
		//d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;

		d_neurons->p_vm[nid] = d_neurons->p_Cm[nid] * d_neurons->p_vm[nid] + d_neurons->p_v_tmp[nid] + d_neurons->p_i_E[nid] * d_neurons->p_C_E[nid] + d_neurons->p_i_I[nid] * d_neurons->p_C_I[nid];

		//d_neurons->p_i_syn[nid] = 0;

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
	//__syncthreads();
	//if (threadIdx.x == 0 && blockIdx.x == 0) {
	//	gActiveTableSize = 0;
	//}
}

__global__ void update_all_life_neuron(GLIFENeurons *d_neurons, int num, int start_id)
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

		int nid = idx;
		int gnid = start_id + idx; 
		bool actived = d_neurons->p_refrac_step[idx] <= 0;

		if (actived) {
			//real I = sqrtf(d_neurons->p_CE[nid]) * d_neurons->p_i_E[nid] + sqrtf(d_neurons->p_CI[nid]) * d_neurons->p_i_I[nid] + d_neurons->p_i_tmp[nid];

			//real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[nid];
			//d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
			//d_neurons->p_i_syn[nid] = 0;

			d_neurons->p_vm[nid] = d_neurons->p_Cm[nid] * d_neurons->p_vm[nid] + d_neurons->p_v_tmp[nid] + d_neurons->p_i_E[nid] * d_neurons->p_C_E[nid] + d_neurons->p_i_I[nid] * d_neurons->p_C_I[nid];

			gXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];

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
				d_neurons->p_i_E[nid] += gNeuronInput[gnid];
				d_neurons->p_i_I[nid] += gNeuronInput_I[gnid];
			}

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
		} else {
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
		}
		gNeuronInput[gnid] = 0;
		gNeuronInput_I[gnid] = 0;
	}
	__syncthreads();
}

__global__ void update_dense_life_neuron(GLIFENeurons *d_neurons, int num, int start_id)
{
	//__shared__ int fire_table_t[MAXBLOCKSIZE];
	//__shared__ volatile int fire_cnt;

	//if (threadIdx.x == 0) {
	//	fire_cnt = 0;
	//}
	//__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		//bool fired = false;
		//int test_loc = 0;

		int nid = idx;
		int gnid = start_id + idx; 
		bool actived = d_neurons->p_refrac_step[idx] <= 0;

		if (actived) {
			d_neurons->p_vm[nid] = d_neurons->p_Cm[nid] * d_neurons->p_vm[nid] + d_neurons->p_v_tmp[nid] + d_neurons->p_i_E[nid] * d_neurons->p_C_E[nid] + d_neurons->p_i_I[nid] * d_neurons->p_C_I[nid];

			d_neurons->p_i_E[nid] *= d_neurons->p_CE[nid];
			d_neurons->p_i_I[nid] *= d_neurons->p_CI[nid];

			bool fired = d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid];

			gFiredTable[gFiredTableCap*gCurrentIdx + gnid] = fired;

			gFireCount[gnid] += fired;

			if (fired) {
				d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
				d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];

			} else {
				gXInput[gnid] += gNeuronInput[gnid] + gNeuronInput_I[gnid];
				d_neurons->p_i_E[nid] += gNeuronInput[gnid];
				d_neurons->p_i_I[nid] += gNeuronInput_I[gnid];
				//real input = 0, input_I = 0;
				//for (int i=d_neurons->p_start_E[nid]; i<d_neurons->p_start_I[nid]; i++) {
				//	input += gNeuronInput[i];
				//}
				//for (int i=d_neurons->p_start_I[nid]; i<d_neurons->p_end[nid]; i++) {
				//	input_I += gNeuronInput[i];
				//}
				//d_neurons->p_i_E[nid] += input;
				//d_neurons->p_i_I[nid] += input_I;
				//gXInput[gnid] += input + input_I;
			}

		} else {
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
			gFiredTable[gFiredTableCap*gCurrentIdx + gnid] = 0;
		}
		gNeuronInput[gnid] = 0;
		gNeuronInput_I[gnid] = 0;
	}
	__syncthreads();
}

__global__ void update_pre_synapse(N2SConnection *pConnection)
{
	__shared__ int fire_neuron_id[MAXBLOCKSIZE];
	__shared__ int fire_synapse_id[MAXBLOCKSIZE];
	__shared__ volatile int fire_cnt;
	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int block_idx = blockIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		int block_nums = (firedSize + blockDim.x - 1) / blockDim.x;
		for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
			block_idx += gridDim.x;
			fire_neuron_id[threadIdx.x] = gFiredTable[time_idx*gFiredTableCap + idx];
			__syncthreads();

			int size = 0;
			if (block_idx == block_nums - 1) {
				size = firedSize - block_idx * blockDim.x;
			} else if (block_idx < block_nums - 1) {
				size = blockDim.x;
			} else {
				size = 0;
			}

			for (int i=0; i<size; i++) {
				int nid =  fire_neuron_id[i];
				int start_loc = pConnection->delayStart[delta_t + nid * MAX_DELAY];
				int synapseNum = pConnection->delayNum[delta_t + nid * MAX_DELAY];
				for (int j=tid; j<synapseNum; j += blockDim.x) {
					int test_loc = atomicAdd((int*)&fire_cnt, 1);
					//fire_synapse_id[threadIdx.x] = pConnection->pSynapsesIdx[j+start_loc];
					fire_synapse_id[threadIdx.x] = j + start_loc;
					__syncthreads();
					//TODO
				}
			}

			//int offset = atomicAdd(&gSynapsesActiveTableSize, synapseNum);
			//gLayerInput[nid]++;
			//for (int i=0; i<synapseNum; i++) {
			//	gSynapsesActiveTable[offset+i] = pConnection->pSynapsesIdx[i+start_loc];
			//}
		}
	}

	//for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
	//	int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
	//	int firedSize = gFiredTableSizes[time_idx];
	//	for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
	//		int nid = gFiredTable[time_idx*gFiredTableCap + idx];
	//		int start_loc = pConnection->delayStart[delta_t + nid * MAX_DELAY];
	//		int synapseNum = pConnection->delayNum[delta_t + nid * MAX_DELAY];
	//		int offset = atomicAdd(&gSynapsesActiveTableSize, synapseNum);
	//		gLayerInput[nid]++;
	//		for (int i=0; i<synapseNum; i++) {
	//			gSynapsesActiveTable[offset+i] = pConnection->pSynapsesIdx[i+start_loc];
	//		}
	//	}
	//}
	__syncthreads();
}

//__global__ void update_exp_hit(GExpSynapses *d_synapses, int num, int start_id)
//{
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
//		int sid = gSynapsesActiveTable[idx];
//		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
//		d_synapses->p_I_syn[sid] += d_synapses->p_weight[sid];
//		gSynapsesLogTable[start_id + sid] = gCurrentCycle;
//		atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), d_synapses->p_I_syn[sid]);
//	}
//	__syncthreads();
//
//}
//
//__global__ void find_exp_synapse(GExpSynapses *d_synapses, int num, int start_id)
//{
//	__shared__ int active_table_t[MAXBLOCKSIZE];
//	__shared__ volatile unsigned int active_cnt;
//
//	if (threadIdx.x == 0) {
//		active_cnt = 0;
//	}
//	__syncthreads();
//
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
//		bool actived = false;
//		int test_loc = 0;
//
//		actived = (gSynapsesLogTable[start_id + idx] < gCurrentCycle);
//
//		for (int i=0; i<2; i++) {
//			if (actived) {
//				test_loc = atomicAdd((int*)&active_cnt, 1);
//				if (test_loc < MAXBLOCKSIZE) {
//					active_table_t[test_loc] = idx;
//					actived = false;
//				}
//			}
//			__syncthreads();
//
//			if (active_cnt >= MAXBLOCKSIZE) {
//				commit2globalTable(active_table_t, MAXBLOCKSIZE, gSynapsesActiveTable, &gSynapsesActiveTableSize, 0);
//				if (threadIdx.x == 0) {
//					active_cnt = 0;
//				}
//			}
//			__syncthreads();
//		}
//	}
//	
//	if (active_cnt > 0) {
//		commit2globalTable(active_table_t, active_cnt, gSynapsesActiveTable, &gSynapsesActiveTableSize, 0);
//		if (threadIdx.x == 0) {
//			active_cnt = 0;
//		}
//	}
//	__syncthreads();
//
//}
//
//__global__ void update_exp_synapse(GExpSynapses *d_synapses, int num, int start_id)
//{
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
//		int sid = gSynapsesActiveTable[idx];
//		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
//
//		if (gSynapsesLogTable[sid] + d_synapses->p_active_steps[sid] <= gCurrentCycle) {
//			atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), d_synapses->p_I_syn[sid]);
//		} else {
//			d_synapses->p_I_syn[sid] = 0; 
//		}
//
//	}
//	__syncthreads();
//}
//
//__global__ void update_all_exp_synapse(GExpSynapses *d_synapses, int num, int start_id)
//{
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
//		if (gSynapsesLogTable[idx] < gCurrentCycle) {
//			d_synapses->p_I_syn[idx] *= d_synapses->p_C1[idx];
//
//			if (gSynapsesLogTable[idx] + d_synapses->p_active_steps[idx] <= gCurrentCycle) {
//				atomicAdd(&(gNeuronInput[d_synapses->p_dst[idx]]), d_synapses->p_I_syn[idx]);
//			} else {
//				d_synapses->p_I_syn[idx] = 0; 
//			}
//		}
//
//	}
//	__syncthreads();
//}

__global__ void update_dense_static_hit(GStaticSynapses *d_synapses, int num, int start_id)
{
#ifndef FAST_TEST
	__shared__ int fire_neuron_id[MAXBLOCKSIZE];

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int block_idx = blockIdx.x;
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		int block_nums_minus_1 = (firedSize - 1 + blockDim.x) / blockDim.x - 1;
		int grid_nums = (firedSize - 1 + blockDim.x*gridDim.x)/(blockDim.x * gridDim.x);
		int oid = tid;
		for (int idx = 0; idx < grid_nums; idx++) {
			if (oid < firedSize) {
				fire_neuron_id[threadIdx.x] = gFiredTable[time_idx*gFiredTableCap + oid];
			} else {
				fire_neuron_id[threadIdx.x] = -1;
			}
			oid += blockDim.x * gridDim.x;
			__syncthreads();

			int size = 0;
			if (block_idx == block_nums_minus_1) {
				size = firedSize - block_idx * blockDim.x;
			} else if (block_idx < block_nums_minus_1) {
				size = blockDim.x;
			} else {
				size = 0;
			}

			for (int i=0; i<size; i++) {
				int nid = fire_neuron_id[i];
				int start_loc = gConnection->delayStart[delta_t + nid * MAX_DELAY];
				int synapseNum = gConnection->delayNum[delta_t + nid * MAX_DELAY];
				for (int j=threadIdx.x; j<synapseNum; j += blockDim.x) {
					//int sid = gConnection->pSynapsesIdx[j+start_loc];
					int sid = j+start_loc;
					real weight = d_synapses->p_weight[sid];
					if (weight >= 0) {
						atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
					} else {
						atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
					}
				}
			}
			block_idx += gridDim.x;
			__syncthreads();
		}
		__syncthreads();
	}
#else
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
			int nid = gFiredTable[time_idx*gFiredTableCap + idx];
			int start_loc = gConnection->delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = gConnection->delayNum[delta_t + nid * MAX_DELAY];
			gLayerInput[nid]++;
			for (int i=0; i<synapseNum; i++) {
				//int sid = gConnection->pSynapsesIdx[i+start_loc];
				int sid = i+start_loc;
				real weight = d_synapses->p_weight[sid];
				if (weight >= 0) {
					atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
				} else {
					atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
				}
			}
		}
	}
#endif
}

//__global__ void update_static_hit(GStaticSynapses *d_synapses, int num, int start_id)
//{
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
//		int sid = gSynapsesActiveTable[idx];
//		real weight = d_synapses->p_weight[sid];
//		gSynapsesLogTable[start_id + sid] = gCurrentCycle;
//		if (weight >= 0) {
//			atomicAdd(&(gNeuronInput[d_synapses->p_dst[sid]]), weight);
//		} else {
//			atomicAdd(&(gNeuronInput_I[d_synapses->p_dst[sid]]), weight);
//		}
//	}
//	__syncthreads();
//
//}

__global__ void add_cross_neuron(int *ids, int num)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	if (tid < num) {
		gFiredTable[gFiredTableCap*gCurrentIdx + gFiredTableSizes[gCurrentIdx] + tid] = ids[tid];
	}
	__syncthreads();

	if (tid == 0) {
		gFiredTableSizes[gCurrentIdx] += num;
	}
}

//__global__ void update_basic_synapse(GBasicSynapses *d_synapses, int num, int start_id)
//{
//	__syncthreads();
//}
//
//__global__ void update_alpha_synapse(GAlphaSynapses *d_synapses, int num, int start_id)
//{
//	int tid = blockIdx.x * blockDim.x + threadIdx.x;
//	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
//		int sid = idx;
//		if (sid < num) {
//			d_synapses->p_I_syn[sid] = d_synapses->p_C1[sid] * d_synapses->p_I_syn[sid] + d_synapses->p_C2[sid] * d_synapses->p_I_tmp[sid];
//			d_synapses->p_I_tmp[sid] *= d_synapses->p_C1[sid];
//		}
//	}
//	__syncthreads();
//}

__global__ void init_connection(N2SConnection *pConnection)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gConnection = pConnection;
	}
}

__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, real *c_gNeuronInput_I, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesActiveTable, int *c_gSynapsesLogTable) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gCurrentIdx = 0;
		gCurrentCycle = 0;
		gFiredTableSize = 0;
		gActiveTableSize = 0;
		gSynapsesActiveTableSize = 0;

		//gTimeTable = c_gTimeTable;
		gNeuronInput = c_gNeuronInput;
		gNeuronInput_I = c_gNeuronInput_I;
		gFiredTable = c_gFiredTable;
		gFiredTableSizes = c_gFiredTableSizes;
		gActiveTable = c_gActiveTable;
		//gSynapsesActiveTable = c_gSynapsesActiveTable;
		//gSynapsesLogTable = c_gSynapsesLogTable;
	}
}

__global__ void init_log_buffers(int * layer_input, real * x_input, int * fire_count)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gLayerInput = layer_input;
		gXInput = x_input;
		gFireCount = fire_count;
	}
}

GBuffers* alloc_buffers(int neuron_num, int synapse_num, int max_delay) 
{
	GBuffers *ret = (GBuffers*)malloc(sizeof(GBuffers));
	memset(ret, 0, sizeof(GBuffers));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gNeuronInput), sizeof(real)*(neuron_num)));
	checkCudaErrors(cudaMemset(ret->c_gNeuronInput, 0, sizeof(real)*(neuron_num)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gNeuronInput_I), sizeof(real)*(neuron_num)));
	checkCudaErrors(cudaMemset(ret->c_gNeuronInput_I, 0, sizeof(real)*(neuron_num)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gFiredTable), sizeof(int)*((neuron_num)*(max_delay+1))));
	checkCudaErrors(cudaMemset(ret->c_gFiredTable, 0, sizeof(int)*((neuron_num)*(max_delay+1))));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gFiredTableSizes), sizeof(int)*(max_delay+1)));
	checkCudaErrors(cudaMemset(ret->c_gFiredTableSizes, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gActiveTable), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMemset(ret->c_gActiveTable, 0, sizeof(int)*(neuron_num)));

	//checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesActiveTable), sizeof(int)*(synapse_num)));
	//checkCudaErrors(cudaMemset(ret->c_gSynapsesActiveTable, 0, sizeof(int)*(synapse_num)));

	//checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesLogTable), sizeof(int)*(synapse_num)));
	//checkCudaErrors(cudaMemset(ret->c_gSynapsesLogTable, 0, sizeof(int)*(synapse_num)));

	ret->c_gLayerInput = gpuMalloc<int>(neuron_num);
	ret->c_gXInput = gpuMalloc<real>(neuron_num);
	ret->c_gFireCount = gpuMalloc<int>(neuron_num);

	int timeTableCap = max_delay+1;
	checkCudaErrors(cudaMemcpyToSymbol(MAX_DELAY, &max_delay, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gTimeTableCap, &timeTableCap, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gFiredTableCap, &neuron_num, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gSynapsesTableCap, &synapse_num, sizeof(int)));
	//checkCudaErrors(cudaMalloc((void**)&ret->c_gTimeTable, sizeof(int)*(max_delay+1)));
	//checkCudaErrors(cudaMemset(ret->c_gTimeTable, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMallocHost((void**)(&ret->c_neuronsFired), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMallocHost((void**)(&ret->c_synapsesFired), sizeof(int)*(synapse_num)));

	init_buffers<<<1, 1, 0>>>(/*ret->c_gTimeTable,*/ ret->c_gNeuronInput, ret->c_gNeuronInput_I, ret->c_gFiredTable, ret->c_gFiredTableSizes, ret->c_gActiveTable, ret->c_gSynapsesActiveTable, ret->c_gSynapsesLogTable);

	init_log_buffers<<<1, 1, 0>>>(ret->c_gLayerInput, ret->c_gXInput, ret->c_gFireCount);

	return ret;
}

void init_buffers(GBuffers * buf) {
	init_buffers<<<1, 1, 0>>>(/*buf->c_gTimeTable,*/ buf->c_gNeuronInput, buf->c_gNeuronInput_I, buf->c_gFiredTable, buf->c_gFiredTableSizes, buf->c_gActiveTable, buf->c_gSynapsesActiveTable, buf->c_gSynapsesLogTable);

	init_log_buffers<<<1, 1, 0>>>(buf->c_gLayerInput, buf->c_gXInput, buf->c_gFireCount);
}

int free_buffers(GBuffers *buf) 
{
	checkCudaErrors(cudaFree(buf->c_gNeuronInput));
	checkCudaErrors(cudaFree(buf->c_gNeuronInput_I));
	checkCudaErrors(cudaFree(buf->c_gFiredTable));
	checkCudaErrors(cudaFree(buf->c_gFiredTableSizes));
	checkCudaErrors(cudaFree(buf->c_gActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesLogTable));

	checkCudaErrors(cudaFreeHost(buf->c_neuronsFired));
	checkCudaErrors(cudaFreeHost(buf->c_synapsesFired));

	return 0;
}
