
#include "gpu_kernel.h"

#define MAX_FIRED 512

__device__ PlainNetwork * pGpuNet;
__device__ unsigned int *gTimeTable;
__device__ bool *gSynapsesFiredTable;
__device__ unsigned int *gFiredTable;
__device__ unsigned int gFiredCnt;
__device__ unsigned int gFiredCntTest;

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
	if ((threadIdx.x == 0) && (blockDim.x == 0)) {
		gTimeTable[simTime + pGpuNet->MAX_DELAY + 1] = gFiredCnt;
	}
	__syncthreads();
	return 0;
}

__device__ int updateFiredTable(int *fireTable, unsigned int fireCnt, unsigned int simTime)
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

__global__ void init_global(PlainNetwork * c_pGpuNet, unsigned int *c_gTimeTable, unsigned int *c_gFiredTable, bool *c_gSynapsesFiredTable) {
	pGpuNet = c_pGpuNet;
	gTimeTable = c_gTimeTable;
	gFiredTable = c_gFiredTable;
	gSynapsesFiredTable = c_gSynapsesFiredTable;
	gFiredCnt = 0;
	gFiredCntTest = 0;
}

__global__ void update_postSynapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, unsigned int simTime)
{
	//int tid = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int idx_s = gTimeTable[simTime];	
	unsigned int idx_e = gTimeTable[simTime+pGpuNet->MAX_DELAY];
	for (unsigned int idx = idx_s + threadIdx.x; idx <= idx_e; idx += blockDim.x) {
		unsigned int nid = gFiredTable[idx];
		for (unsigned int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
			unsigned int loc = d_neurons->pSynapsesLoc[nid];
			gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
		}
	}
	__syncthreads();
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, unsigned int simTime)
{
	__shared__ volatile unsigned int fireCnt;
	__shared__ volatile unsigned int fireCntTest;
	__shared__ int fireTable[MAX_FIRED];

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

	if (fireCnt) {
		updateFiredTable(fireTable, fireCnt, simTime);
	}
	__syncthreads();
}

__global__ void update_alpha_synapse(GAlphaSynapses *d_synapses, unsigned int num, unsigned int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int sid = tid;
	if (sid < num) {
		d_synapses->p_I_syn[sid] = d_synapses->p_C1[sid] * d_synapses->p_I_syn[sid] + d_synapses->p_C2[sid] * d_synapses->p_I_tmp[sid];
		d_synapses->p_I_tmp[sid] *= d_synapses->p_C1[sid];
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
			atomicAdd(&(d_neurons->p_vm[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
		}
	}
	__syncthreads();
}
