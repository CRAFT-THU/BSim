#include "Neuron.h"
#include "Synapse.h"

#define MAX_DELAY 10
#define MAX_FIRED 1000
#define ONESECOND 1000

__device__ unsigned int gTimeTable[ONESECOND];
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
		gTimeTable[simTime + MAX_DELAY + 1] = gFiredCnt;
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
		//check over add items
		cnt = atomicAdd(&gFiredCnt, fireCnt);
	}
	__syncthreads();

	for (int i=threadIdx.x; i<fireCnt; i+=blockDim.x) {
		idx = atomicAdd((unsigned int*)&cnt, 1);
		gFiredTable[idx] = fireTable[i];
	}
	return 0;
}

__global__ void postSynapse()
{

}

__global__ void update_lif_neuron(GLIFNeuron *d_neurons, int num, unsigned int simTime)
{
	__shared__ volatile unsigned int fireCnt;
	__shared__ volatile unsigned int fireCntTest;
	__shared__ int fireTable[MAX_FIRED];

	int idx = 0;
	if (threadIdx.x == 0) {
		fireCnt = 0;
	}

	__syncthreads();

	bool fired = false;
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nid = tid;
	if (nid < num) {
		real I = d_neurons[nid].i_syn + d_neurons[nid].i_tmp;
		d_neurons[nid].vm = d_neurons[nid].vm * d_neurons[nid].C1 + d_neurons[nid].C2 * I; 
		d_neurons[nid].i_syn = 0;

		if (d_neurons[nid].refrac_step > 0) {
			d_neurons[nid].refrac_step --;
			d_neurons[nid].vm = 0;
		}

		if (d_neurons[nid].vm >= d_neurons[nid].v_thresh) {
			fired = true;
			//d_neurons[nid].refrac_step = (int)(d_neurons[nid].tau_refrac/d_neurons[nid]._dt) - 1;
			//d_neurons[nid].vm = d_neurons[nid].v_reset;
		}


	}
	__syncthreads();

	if (fired) {
		idx = atomicAdd((int*)&fireCntTest, 1);
	}
	if (fired && idx < MAX_FIRED) {
		idx = atomicAdd((int*)&fireCnt, 1);
		fired = false;
		fireTable[idx] = nid;
	}
	__syncthreads();
	if (fireCnt) {
		updateFiredTable(fireTable, fireCnt, simTime);
	}
}

__global__ void update_alpha_synapse(GAlphaSynapse *d_synapses, int num)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int sid = tid;
	if (sid < num) {
		d_synapses[sid].I_syn = d_synapses[sid].C1 * d_synapses[sid].I_syn + d_synapses[sid].C2 * d_synapses[sid].I_tmp;
		d_synapses[sid].I_tmp *= d_synapses[sid].C1;
	}
}

__global__ void update_exp_synapse(GExpSynapse *d_synapses, int num)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int sid = tid;
	if (tid < num) {
		d_synapses[sid].I_syn *= d_synapses[sid].C1;
	}
}
