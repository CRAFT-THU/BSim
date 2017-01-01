
#include "gpu_macros.h"
#include "gpu_kernel.h"

#define MAX_FIRED 512

__device__ int MAX_DELAY;
__device__ int gCurrentTime;
__device__ int *gTimeTable;
__device__ int gTimeTableSize;
__device__ real *gNeuronInput;
__device__ int gNeuronNum;
__device__ int *gFiredTable;
__device__ int gFiredTableSize;
__device__ int gFiredCnt;
__device__ int gFiredCntTest;
__device__ bool *gSynapsesFiredTable;
__device__ int gSynapsesFiredTableSize;
__device__ GNetwork *gGpuNet;

__device__ int get_type(int *array, int num, int value, int *offset)
{
	for (int i=0; i<num; i++) {
		if (array[i+1] > value) {
			if (offset != NULL) {
				*offset = value - array[i];
			}

			return i;
		}
	}
	return 0;
}

//int gnid = get_gid(gGpuNet->neuronNums, gGpuNet->nTypes, gGpuNet->nTypeNum, LIF, nid);
//__device__ int get_gid(int *array, Type *types, int num, int type, int offset)
__device__ int type2gnid(GNetwork *net, Type type, int offset)
{
	for (int i=0; i<net->nTypeNum; i++) {
		if (net->nTypes[i] == type) {
			return (net->gNeuronNums[i] + net->nOffsets[i] + offset);
		}
	}

	return 0;
}

__device__ int get_gnid(GNetwork *net, int type, int offset)
{
	return (net->gNeuronNums[type] + net->nOffsets[type] + offset);
}

__device__ int get_gsid(GNetwork *net, int type, int offset)
{
	return (net->gSynapseNums[type] + net->sOffsets[type] + offset);
	//for (int i=0; i<net->sTypeNum; i++) {
	//	if (net->sTypes[i] == type) {
	//		return (net->gSynapseNums[i] + net->sOffsets[i] + offset);
	//	}
	//}

	//return 0;
}

//__device__ double atomicAdd(double *address, double val)
//{
//	unsigned long long int *address_as_ull = (unsigned long long int*)address;
//	unsigned long long int old = *address_as_ull, assumed = 0;
//	do {
//		assumed = old;
//		old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val + __longlong_as_double(assumed)));
//	} while (assumed != old);
//
//	return __longlong_as_double(old);
//}

__device__ int updateTimeTable(int simTime)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gTimeTable[gCurrentTime] = simTime;
		gCurrentTime = (gCurrentTime +1)%(MAX_DELAY + 1);
	}
	__syncthreads();
	return 0;
}

__device__ int updateFiredTable(int firedID, int fired, int simTime)
{
	gFiredTable[gCurrentTime*gFiredTableSize + firedID] = fired;

	return 0;
}

__global__ void init_global(int max_delay, int *c_gTimeTable, real *c_gNeuronInput, int *c_gFiredTable, int c_gFiredTableSize, bool *c_gSynapsesFiredTable, int c_gSynapsesFiredTableSize, GNetwork* network) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		MAX_DELAY = max_delay;
		gCurrentTime = 0;
		gTimeTable = c_gTimeTable;
		gTimeTableSize = MAX_DELAY + 1;
		gNeuronInput = c_gNeuronInput;
		gNeuronNum = c_gFiredTableSize;
		gFiredTable = c_gFiredTable;
		gFiredTableSize = c_gFiredTableSize;
		gSynapsesFiredTable = c_gSynapsesFiredTable;
		gSynapsesFiredTableSize = c_gSynapsesFiredTableSize;
		gFiredCnt = 0;
		gFiredCntTest = 0;
		gGpuNet = network;
	}
}


//__device__ int get_alpha_delay(void *data, int num, int sid)
//{
//	GAlphaSynapses *d_synapses = (GAlphaSynapses*)data;
//	return (int)(d_synapses->p_delay_steps[sid]);
//}
//
//__device__ int get_exp_delay(void *data, int num, int sid)
//{
//	GExpSynapses *d_synapses = (GExpSynapses*)data;
//	return (int)(d_synapses->p_delay_steps[sid]);
//}

GET_DELAY(basic, Basic)
GET_DELAY(alpha, Alpha)
GET_DELAY(exp, Exp)

__device__ int (*get_delay[])(void *, int, int) = { NULL, NULL, get_basic_delay, get_alpha_delay, get_exp_delay };

__device__ int update_constant_spike(void *data, int num, int nid, int start_t, int simTime)
{
	if (nid >= num) {
		return -1;
	}

	GConstantNeurons *d_neurons = (GConstantNeurons*)data;
	for (int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
		int loc = d_neurons->pSynapsesLoc[nid];
		int sid = d_neurons->pSynapsesIdx[i+loc];
		int offset = 0;
		int type = get_type(gGpuNet->synapseNums, gGpuNet->sTypeNum, sid, &offset);
		//if (simTime == start_t + get_delay(d_synapses->p_delay[sid]/d_synapses->p__dt[sid]))
		if (simTime == start_t + get_delay[gGpuNet->sTypes[type]](gGpuNet->pSynapses[type], gGpuNet->synapseNums[type+1]-gGpuNet->synapseNums[type], offset))
			gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
	}

	return 0;
}

__device__ int update_lif_spike(void *data, int num, int nid, int start_t, int simTime)
{
	if (nid >= num) {
		return -1;
	}

	GLIFNeurons *d_neurons = (GLIFNeurons*)data;
	for (int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
		int loc = d_neurons->pSynapsesLoc[nid];
		int sid = d_neurons->pSynapsesIdx[i+loc];
		int offset = 0;
		int type = get_type(gGpuNet->synapseNums, gGpuNet->sTypeNum, sid, &offset);
		//if (simTime == start_t + get_delay(d_synapses->p_delay[sid]/d_synapses->p__dt[sid]))
		if (simTime == start_t + get_delay[gGpuNet->sTypes[type]](gGpuNet->pSynapses[type], gGpuNet->synapseNums[type+1]-gGpuNet->synapseNums[type], offset))
			gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
	}

	return 0;
}

//UPDATE_NEURON_SPIKE(constant, Constant)
//UPDATE_NEURON_SPIKE(lif, LIF)

__device__ int update_basic_spike(void *data, int num, int sid, int start_t, int simTime)
{
	if (sid >= num) {
		return -1;
	}

	GBasicSynapses *d_synapses = (GBasicSynapses*)data;
	atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_weight[sid]);

	return 0;
}

__device__ int update_alpha_spike(void *data, int num, int sid, int start_t, int simTime)
{
	if (sid >= num) {
		return -1;
	}

	GAlphaSynapses *d_synapses = (GAlphaSynapses*)data;
	real I_t = d_synapses->p__C2[sid] * d_synapses->p_I_syn[sid] + d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid];
	d_synapses->p_I_tmp[sid] += d_synapses->p_weight[sid]/d_synapses->p__C1[sid];
	d_synapses->p_I_syn[sid] = (I_t - d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid])/d_synapses->p__C1[sid];
	//atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
	atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);

	return 0;
}

__device__ int update_exp_spike(void *data, int num, int sid, int start_t, int simTime)
{
	if (sid >= num) {
		return -1;
	}

	GExpSynapses *d_synapses = (GExpSynapses*)data;
	d_synapses->p_I_syn[sid] += (d_synapses->p_weight[sid]/d_synapses->p__C1[sid]);
	//atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
	atomicAdd(&(gNeuronInput[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);

	return 0;
}


__device__ int (*update_spike[])(void*, int, int, int, int) = { update_constant_spike, update_lif_spike, update_basic_spike, update_alpha_spike, update_exp_spike };

__global__ void update_pre_synapse(GNetwork *d_net, int simTime)
{
	for (int time = 0; time<MAX_DELAY+1; time++) {
		int tid = blockIdx.x * blockDim.x + threadIdx.x;
		int start_t = gTimeTable[time];
		for (int idx = tid; idx < gFiredTableSize; idx += blockDim.x*gridDim.x) {
			int nid = idx;
			int offset = 0;
			int type = get_type(d_net->neuronNums, d_net->nTypeNum, nid, &offset);
			int gnid = get_gnid(d_net, type, offset);
			if (gFiredTable[time*gFiredTableSize + gnid] > 0) {
				update_spike[d_net->nTypes[type]](d_net->pNeurons[type], d_net->neuronNums[type+1]-d_net->neuronNums[type], offset, start_t, simTime);
			}
		}
	}
	__syncthreads();
}

__global__ void update_post_synapse(GNetwork *d_net, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int num = d_net->synapseNums[d_net->sTypeNum];
	for (int idx = tid; idx<num; idx += blockDim.x*gridDim.x) {
		int sid = idx;
		int offset = 0;
		int type = get_type(d_net->synapseNums, d_net->sTypeNum, sid, &offset);
		int gsid = get_gsid(d_net, type, offset);
		if (gSynapsesFiredTable[gsid]) {
			update_spike[d_net->sTypes[type]](d_net->pSynapses[type], d_net->synapseNums[type+1]-d_net->neuronNums[type], offset, 0, simTime);
			gSynapsesFiredTable[gsid] = false;
		}
	}
}

__global__ void update_pre_synapse(GLIFNeurons *d_neurons, GExpSynapses* d_synapses, int simTime)
{
	for (int time = 0; time<MAX_DELAY+1; time++) {
		int tid = blockIdx.x * blockDim.x + threadIdx.x;
		int start_t = gTimeTable[time];
		for (int idx = tid; idx < gFiredTableSize; idx += blockDim.x*gridDim.x) {
			int nid = idx;
			if (gFiredTable[time*gFiredTableSize + nid]) {
				for (int i=0; i<d_neurons->pSynapsesNum[nid]; i++) {
					int loc = d_neurons->pSynapsesLoc[nid];
					int sid = d_neurons->pSynapsesIdx[i+loc];
					if (simTime == start_t + d_synapses->p_delay_steps[sid])
						gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true;
				}
			}
		}
	}
	__syncthreads();
}

__global__ void update_constant_neuron(GConstantNeurons *d_neurons, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nid = 0;
	for (int idx = tid; idx < num; idx += blockDim.x * gridDim.x) {
		nid = idx;
		if (nid < num) {
			int fired = (simTime * d_neurons->p_fire_rate[nid]) > (d_neurons->p_fire_count[nid]);
			int gnid = type2gnid(gGpuNet, Constant, nid);
			if (fired) {
				d_neurons->p_fire_count[nid]++;
			}
			updateFiredTable(gnid, fired, simTime);
		}
	}
}

__global__ void update_lif_neuron(GLIFNeurons *d_neurons, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int nid = 0;
	for (int idx = tid; idx < num; idx +=blockDim.x*gridDim.x) {
		nid = idx;
		if (nid < num) {
			int fired = 0;
			//real I = d_neurons->p_i_syn[nid] + d_neurons->p_i_tmp[nid];
			int gnid = type2gnid(gGpuNet, LIF, nid);
			real I = gNeuronInput[gnid] + d_neurons->p_i_tmp[nid];
			d_neurons->p_vm[nid] = d_neurons->p_vm[nid] * d_neurons->p_C1[nid] + d_neurons->p_C2[nid] * I;
			//d_neurons->p_i_syn[nid] = 0;
			gNeuronInput[gnid] = 0;

			if (d_neurons->p_refrac_step[nid] > 0) {
				d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
				d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
			} else if (d_neurons->p_vm[nid] >= d_neurons->p_v_thresh[nid]) {
				fired = 1;
				d_neurons->p_refrac_step[nid] = d_neurons->p_refrac_time[nid] - 1;
				d_neurons->p_vm[nid] = d_neurons->p_v_reset[nid];
			}

			updateFiredTable(gnid, fired, simTime);
		}
	}
	__syncthreads();

	updateTimeTable(simTime);
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

			
			//if (gSynapsesFiredTable[sid]) {
			//	real I_t = d_synapses->p__C2[sid] * d_synapses->p_I_syn[sid] + d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid];
			//	d_synapses->p_I_tmp[sid] += d_synapses->p_weight[sid]/d_synapses->p__C1[sid];
			//	d_synapses->p_I_syn[sid] = (I_t - d_synapses->p__C2[sid] * d_synapses->p_I_tmp[sid])/d_synapses->p__C1[sid];
			//	atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
			//	gSynapsesFiredTable[sid] = false;
			//}
		}
	}
	__syncthreads();
}

__global__ void update_exp_synapse(GExpSynapses *d_synapses, int num, int simTime)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	for (int idx = tid; idx < num; idx += blockDim.x*gridDim.x) {
		int sid = idx;
		if (sid < num) {
			d_synapses->p_I_syn[sid] *= d_synapses->p_C1[sid];
		}

		//if (gSynapsesFiredTable[sid]) {
		//	d_synapses->p_I_syn[sid] += (d_synapses->p_weight[sid]/d_synapses->p__C1[sid]);
		//	atomicAdd(&(d_neurons->p_i_syn[d_synapses->pDst[sid]]), d_synapses->p_I_syn[sid]);
		//	gSynapsesFiredTable[sid] = false;
		//}
	}
	__syncthreads();
}
