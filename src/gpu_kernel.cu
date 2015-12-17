
#include "gpu_kernel.h"

#define MAX_FIRED 512

__device__ unsigned int MAX_DELAY;
__device__ unsigned int *gTimeTable;
__device__ unsigned int gTimeTableSize;
__device__ unsigned int *gFiredTable;
__device__ unsigned int gFiredTableSize;
__device__ unsigned int gFiredCnt;
__device__ unsigned int gFiredCntTest;
__device__ bool *gSynapsesFiredTable;
__device__ unsigned int gSynapsesFiredTableSize;

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

__device__ int updateTimeTable(unsigned int simTime)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gTimeTable[simTime + MAX_DELAY + 1] = gFiredCnt;
	}
	__syncthreads();
	return 0;
}

__device__ int updateFiredTable(unsigned int *fireTable, unsigned int fireCnt, unsigned int simTime)
{
	__shared__ volatile unsigned int cnt;
	unsigned int idx = 0;
	if (threadIdx.x == 0) {
		cnt = atomicAdd(&gFiredCntTest, fireCnt);
		//TODO: check over add items
		cnt = atomicAdd(&gFiredCnt, fireCnt);
	}
	__syncthreads();

	for (int i=threadIdx.x; i<fireCnt; i+=blockDim.x) {
		idx = atomicAdd((unsigned int*)&cnt, 1);
		gFiredTable[idx] = fireTable[i];
	}
	return 0;
}

__global__ void init_global(unsigned int max_delay, unsigned int *c_gTimeTable, unsigned int c_gTimeTableSize, unsigned int *c_gFiredTable, unsigned int c_gFiredTableSize, bool *c_gSynapsesFiredTable, unsigned int c_gSynapsesFiredTableSize) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		MAX_DELAY = max_delay;
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

__global__ void update_pre_synapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, unsigned int simTime)
{
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int idx_s = gTimeTable[simTime];	
	unsigned int idx_e = gTimeTable[simTime+MAX_DELAY];
	for (unsigned int idx = idx_s + threadIdx.x; idx <= idx_e; idx += blockDim.x) {
		unsigned int nid = gFiredTable[idx];
		unsigned int t = 0;
		for (t=max(0, simTime-MAX_DELAY-1); t<simTime+MAX_DELAY+1; t++) {
			if (gTimeTable[t+MAX_DELAY+1] > idx) {
				break;
			}
		}
		if (simTime == t) {
			d_neurons->p_refrac_step[nid]= (int)(d_neurons->p_tau_refrac[nid]/d_neurons->p__dt[nid]) - 1;
			d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];

		}
		for (unsigned int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
			unsigned int loc = d_neurons->pSynapsesLoc[nid];
			unsigned int sid = d_neurons->pSynapsesIdx[i+loc];
			if (simTime == t+(unsigned int)(d_synapses->p_delay[sid]/d_synapses->p__dt[sid]))
					gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
		}

	}
	__syncthreads();
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, unsigned int simTime)
{
	__shared__ volatile unsigned int fireCnt;
	__shared__ volatile unsigned int fireCntTest;
	__shared__ unsigned int fireTable[MAX_FIRED];

	int idx = 0;
	if (threadIdx.x == 0) {
		fireCnt = 0;
		fireCntTest = 0;
	}

	__syncthreads();

	bool fired = false;
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nid = tid;
	if (nid < num) {
		real I = d_neurons->p_i_syn[nid] + d_neurons->p_i_tmp[nid];
		d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
		d_neurons->p_i_syn[nid] = 0;

		if (d_neurons->p_refrac_step[nid] > 0) {
			d_neurons->p_refrac_step[nid] --;
			d_neurons->p_vm[nid] = 0;
		}

		if (d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid]) {
			fired = true;
			//d_neurons[nid].refrac_step = (int)(d_neurons[nid].tau_refrac/d_neurons[nid]._dt) - 1;
			//d_neurons[nid].vm = d_neurons[nid].v_reset;
		}


	}
	__syncthreads();

	if (fired) {
		idx = atomicAdd((unsigned int *)&fireCntTest, 1);
	}
	if (fired && idx < MAX_FIRED) {
		idx = atomicAdd((unsigned int *)&fireCnt, 1);
		fired = false;
		fireTable[idx] = nid;
	}
	__syncthreads();

	if (fireCnt > 0) {
		updateFiredTable(fireTable, fireCnt, simTime);
	}
	__syncthreads();

	updateTimeTable(simTime);
	__syncthreads();
}

__global__ void update_alpha_synapse(GLIFNeurons *d_neurons, GAlphaSynapses *d_synapses, unsigned int num, unsigned int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int sid = tid;
	if (sid < num) {
		d_synapses->p_I_syn[sid] = d_synapses->p_C1[sid] * d_synapses->p_I_syn[sid] + d_synapses->p_C2[sid] * d_synapses->p_I_tmp[sid];
		d_synapses->p_I_tmp[sid] *= d_synapses->p_C1[sid];
	}
	__syncthreads();
	if (sid < num) {
		if (gSynapsesFiredTable[sid]) {
			real I_t = d_synapses->p_C2[sid] * d_synapses->p_I_syn[sid] + d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid];
			d_synapses->p_I_tmp[sid] += d_synapses->p_weight[sid]/d_synapses->p__C1[sid];
			d_synapses->p_I_syn[sid] = (I_t - d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid])/d_synapses->p__C1[sid];
			atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
			gSynapsesFiredTable[sid] = false;
		}
	}
	__syncthreads();
}

__global__ void update_exp_synapse(GLIFNeurons *d_neurons, GExpSynapses *d_synapses, unsigned int num, unsigned int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int sid = tid;
	if (sid < num) {
		d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
	}
	__syncthreads();

	if (sid < num) {
		if (gSynapsesFiredTable[sid]) {
			d_synapses->p_I_syn[sid] += (d_synapses->p_weight[sid]/d_synapses->p__C1[sid]);
			atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
			gSynapsesFiredTable[sid] = false;
		}
	}
	__syncthreads();
}
