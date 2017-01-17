
#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "gpu_macros.h"
#include "gpu_kernel.h"

#define SHARED_SIZE 512

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

// Neuron Tables
__device__ int *gFiredTable;
__device__ int *gFiredTableSizes;

__device__ int *gActiveTable;

// Synapse Tables
__device__ int *gSynapsesActiveTable;

__device__ int *gSynapsesLogTable;



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

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int start_id)
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
			fired = (gCurrentCycle * d_neurons->p_fire_rate[idx]) > (d_neurons->p_fire_count[idx]);
		}

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
				if (test_loc < SHARED_SIZE) {
					fire_table_t[test_loc] = start_id + idx;
					fired = false;
				}
			}
			__syncthreads();
			if (fire_cnt >= SHARED_SIZE) {
				commit2globalTable(fire_table_t, SHARED_SIZE, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
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

__global__ void find_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		bool fired = false;
		int test_loc = 0;

		if (idx < num) {
			fired = d_neurons->p_refrac_step[idx] <= 0;
		}

		if (fired) {
			test_loc = atomicAdd((int*)&fire_cnt, 1);
			if (test_loc < SHARED_SIZE) {
				fire_table_t[test_loc] = start_id + idx;
				fired = false;
			}
		} else {
			gNeuronInput[start_id + idx] = 0;
			d_neurons->p_refrac_step[idx] = d_neurons->p_refrac_step[idx] - 1;
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
			test_loc = atomicAdd((int*)&fire_cnt, 1);
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

__device__ void reset_lif_neuron(GLIFNeurons *d_neurons, int *shared_buf, volatile int size) 
{
	for (int idx=threadIdx.x; idx<size; idx+=blockDim.x) {
		int nid = shared_buf[idx];
		d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
		d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
	}
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int start_id)
{
	__shared__ int fire_table_t[SHARED_SIZE];
	__shared__ volatile int fire_cnt;

	if (threadIdx.x == 0) {
		fire_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
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
					test_loc = atomicAdd((int*)&fire_cnt, 1);
					if (test_loc < SHARED_SIZE) {
						fire_table_t[test_loc] = nid;
						fired = false;
					}
				}
				__syncthreads();
				if (fire_cnt >= SHARED_SIZE) {
					commit2globalTable(fire_table_t, SHARED_SIZE, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
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
			commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &gFiredTableSizes[gCurrentIdx], gFiredTableCap*gCurrentIdx);
			reset_lif_neuron(d_neurons, fire_table_t, SHARED_SIZE);
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

__global__ void update_pre_synapse(N2SConnection *pConnection)
{
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int delta_t = 0; delta_t<MAX_DELAY; delta_t++) {
		int time_idx = (gCurrentIdx+MAX_DELAY-delta_t)%(MAX_DELAY+1);
		int firedSize = gFiredTableSizes[time_idx];
		for (int idx = tid; idx < firedSize; idx += blockDim.x*gridDim.x) {
			int nid = gFiredTable[time_idx*gFiredTableCap + idx];
			int start_loc = pConnection->delayStart[delta_t + nid * MAX_DELAY];
			int synapseNum = pConnection->delayNum[delta_t + nid * MAX_DELAY];
			int offset = atomicAdd(&gSynapsesActiveTableSize, synapseNum);
			for (int i=0; i<synapseNum; i++) {
				gSynapsesActiveTable[offset+i] = pConnection->pSynapsesIdx[i+start_loc];
			}
		}
	}
	__syncthreads();
}

__global__ void update_exp_hit(GExpSynapses *d_synapses, int num, int start_id)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
		int sid = gSynapsesActiveTable[idx];
		d_synapses->p_I_syn[sid] += d_synapses->p_weight[sid];
		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		gSynapsesLogTable[sid] = gCurrentCycle;
		atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
	}
	__syncthreads();

}

__global__ void find_exp_synapse(GLIFNeurons *d_neurons, int num, int start_id)
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
			fired = gSynapsesLogTable[start_id + idx] <= gCurrentCycle;
		}

		for (int i=0; i<2; i++) {
			if (fired) {
				test_loc = atomicAdd((int*)&fire_cnt, 1);
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

__global__ void update_exp_synapse(GExpSynapses *d_synapses, int num, int start_id)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < gSynapsesActiveTableSize; idx += blockDim.x*gridDim.x) {
		int sid = gSynapsesActiveTable[idx];
		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);

	}
	__syncthreads();
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


__global__ void init_buffers(/*int *c_gTimeTable,*/ real *c_gNeuronInput, int *c_gFiredTable, int *c_gFiredTableSizes, int *c_gActiveTable, int *c_gSynapsesActiveTable, int *c_gSynapsesLogTable) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gCurrentIdx = 0;
		gCurrentCycle = 0;
		gFiredTableSize = 0;
		gActiveTableSize = 0;
		gSynapsesActiveTableSize = 0;

		//gTimeTable = c_gTimeTable;
		gNeuronInput = c_gNeuronInput;
		gFiredTable = c_gFiredTable;
		gFiredTableSizes = c_gFiredTableSizes;
		gActiveTable = c_gActiveTable;
		gSynapsesActiveTable = c_gSynapsesActiveTable;
		gSynapsesLogTable = c_gSynapsesLogTable;
	}
}

GBuffers* alloc_buffers(int neuron_num, int synapse_num, int max_delay) 
{
	GBuffers *ret = (GBuffers*)malloc(sizeof(GBuffers));
	
	checkCudaErrors(cudaMalloc((void**)&(ret->c_gNeuronInput), sizeof(real)*(neuron_num)));
	checkCudaErrors(cudaMemset(ret->c_gNeuronInput, 0, sizeof(real)*(neuron_num)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gFiredTable), sizeof(int)*((neuron_num)*(max_delay+1))));
	checkCudaErrors(cudaMemset(ret->c_gFiredTable, 0, sizeof(int)*((neuron_num)*(max_delay+1))));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gFiredTableSizes), sizeof(int)*(max_delay+1)));
	checkCudaErrors(cudaMemset(ret->c_gFiredTableSizes, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gActiveTable), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMemset(ret->c_gActiveTable, 0, sizeof(int)*(neuron_num)));

	checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesActiveTable), sizeof(int)*(synapse_num)));
	checkCudaErrors(cudaMemset(ret->c_gSynapsesActiveTable, 0, sizeof(int)*(synapse_num)));
	
	checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesLogTable), sizeof(int)*(synapse_num)));
	checkCudaErrors(cudaMemset(ret->c_gSynapsesLogTable, 0, sizeof(int)*(synapse_num)));

	int timeTableCap = max_delay+1;
	checkCudaErrors(cudaMemcpyToSymbol(MAX_DELAY, &max_delay, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gTimeTableCap, &timeTableCap, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gFiredTableCap, &neuron_num, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gSynapsesTableCap, &synapse_num, sizeof(int)));
	//checkCudaErrors(cudaMalloc((void**)&ret->c_gTimeTable, sizeof(int)*(max_delay+1)));
	//checkCudaErrors(cudaMemset(ret->c_gTimeTable, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMallocHost((void**)(&ret->c_neuronsFired), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMallocHost((void**)(&ret->c_synapsesFired), sizeof(int)*(synapse_num)));

	init_buffers<<<1, 1, 0>>>(/*ret->c_gTimeTable,*/ ret->c_gNeuronInput, ret->c_gFiredTable, ret->c_gFiredTableSizes, ret->c_gActiveTable, ret->c_gSynapsesActiveTable, ret->c_gSynapsesLogTable);

	return ret;
}

int free_buffers(GBuffers *buf) 
{
	checkCudaErrors(cudaFree(buf->c_gNeuronInput));
	checkCudaErrors(cudaFree(buf->c_gFiredTable));
	checkCudaErrors(cudaFree(buf->c_gFiredTableSizes));
	checkCudaErrors(cudaFree(buf->c_gActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesLogTable));

	checkCudaErrors(cudaFreeHost(buf->c_neuronsFired));
	checkCudaErrors(cudaFreeHost(buf->c_synapsesFired));

	return 0;
}
