
#include "gpu_macros.h"
#include "gpu_kernel.h"

#define SHARED_SIZE 512

// Constant
__constant__ int MAX_DELAY;
__constant__ int gTimeTableCap;
__constant__ int gFiredTableCap;
__constant__ int gSynapsesActiveTableCap;

// Variable
__device__ int gCurrentTime;
__device__ int gFiredTableSize;

// Arrays
__device__ int *gTimeTable;

// Neuron Arrays
__device__ real *gNeuronInput;

// Neuron Tables
__device__ int *gFiredTable;
__device__ int *gFiredTableSizes;

__device__ int *gActiveTable;
__device__ int gActiveTableSize;

// Synapse Tables
__device__ int *gSynapsesActiveTable;
__device__ int *gSynapsesActiveTableSize;

__device__ int *gSynapsesLogTable;



__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, int *global_buf, int * global_size, int offset) 
{
	__shared__ volatile unsigned int start_loc;
	if (threadIdx.x == 0) {
		start_loc = atomicadd(global_size, size);
	}
	__syncthreads();

	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		global_buf[offset + start_loc + idx] = shared_buf[idx];
	}
}

__device__ int updateTimeTable(int simTime)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gTimeTable[gCurrentTime] = simTime;
		gCurrentTime = (gCurrentTime +1)%(MAX_DELAY + 1);
	}
	__syncthreads();
	return 0;
}

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id, int simTime)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		if (idx < num) {
			fired = (simTime * d_neurons->p_fire_rate[nid]) > (d_neurons->p_fire_count[nid]);
		}

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicadd(&fire_cnt, 1);
				if (test_loc < SHARED_SIZE) {
					fire_table_t[test_loc] = start_id + idx;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= SHARED_SIZE) {
				commit2globalTable(fire_table_t, SHARED_SIZE, gFiredTable,&gFiredTableSize, gFiredTableCap*gCurrentTime);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
	}
	__syncthreads();
	
	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gFiredTable,&gFiredTableSize, gFiredTableCap*gCurrentTime);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

__global__ void find_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id, int simTime)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		if (idx < num) {
			fired = d_neurons->p_refrac_step[nid] <= 0;
		}

		if (fired) {
			test_loc = atomicadd(fire_cnt, 1);
			if (test_loc < SHARED_SIZE) {
				fire_table_t[test_loc] = start_id + idx;
				fired = false;
			}
		} else {
			gNeuronInput[start_id + idx] = 0;
			d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_step[nid] - 1;
		}
		__syncthreads();

		if (fire_cnt >= SHARED_SIZE) {
			commit2globalTable(fire_table_t, SHARED_SIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();

		if (fired) {
			test_loc = atomicadd(fire_cnt, 1);
			if (test_loc < SHARED_SIZE) {
				fire_table_t[test_loc] = start_id + idx;
				fired = false;
			}
		}
		__syncthreads();
		if (fire_cnt >= SHARED_SIZE) {
			commit2globalTable(fire_table_t, SHARED_SIZE, gActiveTable, &gActiveTableSize, 0);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
		__syncthreads();
	}
	
	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gActiveTable, &gActiveTableSize, 0);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
	__syncthreads();

}

__device__ void reset_lif_neuron(GLIFNeurons *d_neurons, int *shared_buf, volatile unsigned int size) 
{
	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		int nid = shared_buf[idx];
		d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
		d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
	}
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id, int simTime)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		bool fired = false;
		int test_loc = 0;
		if (idx < gActiveTableSize) {
			int nid = gActiveTable[idx];
			int gnid = start_id + gActiveTable[idx];
			real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[nid];
			d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
			//d_neurons->p_i_syn[nid] = 0;
			gNeuronInput[gnid] = 0;

			fired = d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid];

			for (int i=0; i<2; i++) {
				if (fired) {
					test_loc = atomicadd(&fire_cnt, 1);
					if (test_loc < SHARED_SIZE) {
						fire_table_t[test_loc] = nid;
						fired = false;
					}
				}
				__syncthreads();
				if (fire_cnt >= SHARED_SIZE) {
					commit2globalTable(fire_table_t, SHARED_SIZE, gFiredTable, &gFiredTableSize, gFiredTableCap*gCurrentTime);
					reset_lif_neuron(d_neurons, fire_table_t, SHARED_SIZE);
					if (threadIdx.x == 0) {
						fire_cnt = 0;
					}
				}
				__syncthreads();
			}
		}
		__syncthreads();

		if (fire_cnt > 0) {
			commit2globalTable(fire_table_t, fire_cnt, gFiredTable,&gFiredTableSize, gFiredTableCap*gCurrentTime);
			reset_lif_neuron(d_neurons, fire_cnt, SHARED_SIZE);
			if (threadIdx.x == 0) {
				fire_cnt = 0;
			}
		}
	}
	__syncthreads();
	if (threadIdx.x == 0 && blockIdx.x == 0) {
		gActiveTableSize = 0;
	}
}

__global__ void update_pre_synapse(GNetwork *d_net, int simTime)
{
	updateTimeTable(simTime);
	__syncthreads();
	for (int time = 0; time<MAX_DELAY+1; time++) {
		int tid = blockIdx.x * blockDim.x + threadIdx.x;
		int firedSize = gFiredTableSizes[time];
		int delta_t = simTime - gTimeTable[time]-1;
		if (delta_t < 0) {
			continue;
		}
		for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
			int nid = gFiredTable[time*gFiredTableCap + idx];
			int start_loc = d_net->pN2SConnections[nid].delayStart[delta_t];
			int synapseNum = d_net->pN2SConnections[nid].delayNum[delta_t];
			int offset = atomicadd(&gActiveFiredTableLoc, synapseNum);
			for (int i=0; i<synapseNum; i++) {
				gSynapsesActiveTable[offset+i] = d_net->pN2SConnections[nid].pSynapsesIdx[i+start_loc];
			}
		}
	}
	__syncthreads();
}

__global__ void update_exp_hit(GExpSynapses *d_synapses, int num, int start_id, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
		int sid = gSynapsesActiveTable[idx];
		d_synapses->p_I_syn[sid] += d_synapses->p_weight[sid];
		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		gSynapsesLogTable[sid] = simTime;
		atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid])
	}
	__syncthreads();

}

__global__ void find_exp_synapse(GLIFNeurons *d_neurons, int num, int start_id, int simTime)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile unsigned int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		if (idx < num) {
			fired = gSynapsesLogTable[start_id + idx] <= simTime;
		}

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicadd(fire_cnt, 1);
				if (test_loc < SHARED_SIZE) {
					fire_table_t[test_loc] = start_id + idx;
					fired = false;
				}
			}
			__syncthreads();

			if (fire_cnt >= SHARED_SIZE) {
				commit2globalTable(fire_table_t, SHARED_SIZE, gSynapsesActiveTable, &gSynapsesActiveTableSize, 0);
				if (threadIdx.x == 0) {
					fire_cnt = 0;
				}
			}
			__syncthreads();
		}
	}
	
	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gSynapsesActiveTable, &gSynapsesActiveTableSize, 0);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
	__syncthreads();

}

__global__ void update_exp_synapse(GExpSynapses *d_synapses, int num, int start_id, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
		int sid = gSynapsesActiveTable[idx];
		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid])

	}
	__syncthreads();
}

__global__ void update_basic_synapse(GBasicSynapses *d_synapses, int num, int simTime)
{
	__syncthreads();
}

__global__ void update_alpha_synapse(GAlphaSynapses *d_synapses, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
		int sid = idx;
		if (sid < num) {
			d_synapses->p_I_syn[sid] = d_synapses->p_C1[sid] * d_synapses->p_I_syn[sid] + d_synapses->p_C2[sid] * d_synapses->p_I_tmp[sid];
			d_synapses->p_I_tmp[sid] *= d_synapses->p_C1[sid];
		}
	}
	__syncthreads();
}


__global__ void init_global(int max_delay, int *c_gTimeTable, real *c_gNeuronInput, int *c_gFiredTable, int c_gFiredTableCap, bool *c_gSynapsesActiveTable, int c_gSynapsesActiveTableCap, GNetwork* network) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		MAX_DELAY = max_delay;
		gCurrentTime = 0;
		gTimeTable = c_gTimeTable;
		gTimeTableCap = MAX_DELAY + 1;
		gNeuronInput = c_gNeuronInput;
		gNeuronNum = c_gFiredTableCap;
		gFiredTable = c_gFiredTable;
		gFiredTableSize = 0;
		gFiredTableCap = c_gFiredTableCap;
		gSynapsesActiveTable = c_gSynapsesActiveTable;
		gSynapsesActiveTableCap = c_gSynapsesActiveTableCap;
		gFiredCnt = 0;
		gFiredCntTest = 0;
		//gGpuNet = network;
	}
}
