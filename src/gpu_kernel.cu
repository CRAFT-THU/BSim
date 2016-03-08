
#include "gpu_kernel.h"

#define MAX_FIRED 512

__device__ int MAX_DELAY;
__device__ int gCurrentTime;
__device__ int *gTimeTable;
__device__ int gTimeTableSize;
__device__ bool *gFiredTable;
__device__ int gFiredTableSize;
__device__ int gFiredCnt;
__device__ int gFiredCntTest;
__device__ bool *gSynapsesFiredTable;
__device__ int gSynapsesFiredTableSize;

__device__ double atomicAdd(double *address, double val)
{
	unsigned long long int *address_as_ull = (unsigned long long int*)address;
	unsigned long long int old = *address_as_ull, assumed = 0;
	do {
		assumed = old;
		old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val + __longlong_as_double(assumed)));
	} while (assumed != old);

	return __longlong_as_double(old);
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

__device__ int updateFiredTable(int firedID, bool fired, int simTime)
{
	gFiredTable[gCurrentTime*gFiredTableSize + firedID] = fired;

	return 0;
}

__global__ void init_global(int max_delay, int *c_gTimeTable, int c_gTimeTableSize, bool *c_gFiredTable, int c_gFiredTableSize, bool *c_gSynapsesFiredTable, int c_gSynapsesFiredTableSize) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		MAX_DELAY = max_delay;
		gCurrentTime = 0;
		gTimeTable = c_gTimeTable;
		gTimeTableSize = c_gTimeTableSize;
		gFiredTable = c_gFiredTable;
		gFiredTableSize = c_gFiredTableSize;
		gSynapsesFiredTable = c_gSynapsesFiredTable;
		gSynapsesFiredTableSize = c_gSynapsesFiredTableSize;
		gFiredCnt = 0;
		gFiredCntTest = 0;
	}
}

__global__ void update_pre_synapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, int simTime)
{
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int time = 0; time<MAX_DELAY+1; time++) {
		int tid = blockIdx.x * blockDim.x + threadIdx.x;
		int start_t = gTimeTable[time];
		for (int idx = tid; idx < gFiredTableSize; idx += blockDim.x*gridDim.x) {
			int nid = idx;
			if (gFiredTable[time*gFiredTableSize + nid]) {
				for (int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
					int loc = d_neurons->pSynapsesLoc[nid];
					int sid = d_neurons->pSynapsesIdx[i+loc];
					if (simTime == start_t +(int)(d_synapses->p_delay[sid]/d_synapses->p__dt[sid]))
						gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
				}
			}
		}
	}
	__syncthreads();
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nid = 0;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		nid = idx;
		if (nid < num) {
			bool fired = false;
			real I = d_neurons->p_i_syn[nid] + d_neurons->p_i_tmp[nid];
			d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
			d_neurons->p_i_syn[nid] = 0;

			if (d_neurons->p_refrac_step[nid] > 0) {
				d_neurons->p_refrac_step[nid] --;
				d_neurons->p_vm[nid] = 0;
			}

			if (d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid]) {
				fired = true;
				d_neurons->p_refrac_step[nid] = (int)(d_neurons->p_tau_refrac[nid]/d_neurons->p__dt[nid]) - 1;
				d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
			}
			updateFiredTable(nid, fired, simTime);
		}
	}
	__syncthreads();

	updateTimeTable(simTime);
	__syncthreads();
}

__global__ void update_alpha_synapse(GLIFNeurons *d_neurons, GAlphaSynapses *d_synapses, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
		int sid = tid;
		if (sid < num) {
			d_synapses->p_I_syn[sid] = d_synapses->p_C1[sid] * d_synapses->p_I_syn[sid] + d_synapses->p_C2[sid] * d_synapses->p_I_tmp[sid];
			d_synapses->p_I_tmp[sid] *= d_synapses->p_C1[sid];

			if (gSynapsesFiredTable[sid]) {
				real I_t = d_synapses->p_C2[sid] * d_synapses->p_I_syn[sid] + d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid];
				d_synapses->p_I_tmp[sid] += d_synapses->p_weight[sid]/d_synapses->p__C1[sid];
				d_synapses->p_I_syn[sid] = (I_t - d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid])/d_synapses->p__C1[sid];
				atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
				gSynapsesFiredTable[sid] = false;
			}
		}
	}
	__syncthreads();
}

__global__ void update_exp_synapse(GLIFNeurons *d_neurons, GExpSynapses *d_synapses, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
		int sid = tid;
		if (sid < num) {
			d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		}

		if (gSynapsesFiredTable[sid]) {
			d_synapses->p_I_syn[sid] += (d_synapses->p_weight[sid]/d_synapses->p__C1[sid]);
			atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
			gSynapsesFiredTable[sid] = false;
		}
	}
	__syncthreads();
}
