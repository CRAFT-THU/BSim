
#include <assert.h>

#include "../utils/utils.h"
#include "../neuron/lif/lif.h"
// #include "../neuron/array/array.h"
// #include "../synapse/static/static.h"
// #include "../neuron/constant/constants.h"
// #include "../neuron/decide/decide.h"
// #include "../neuron/fft/fft.h"
// #include "../neuron/max/max.h"
// #include "../neuron/mem/mem.h"
// #include "../neuron/poisson/poisson.h"
// #include "../neuron/tj/tj.h"

#include "mem_op.h"
#include "runtime.h"
// #include "gpu_func.h"

#include "../third_party/cuda/helper_cuda.h"

#define MAXBLOCKSIZE 1024


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


// __constant__ int MAX_DELAY;
__constant__ int gTimeTableCap;
__constant__ int gFiredTableCap;
// __constant__ int gSynapsesTableCap;
__constant__ real DT;

// Variable
// __device__ int gCurrentIdx;
// __device__ int gCurrentCycle;
// __device__ int gFiredTableSize;
// __device__ int gSynapsesActiveTableSize;

// Arrays
//__device__ int *gTimeTable;

// Neuron Arrays
// __device__ real *gNeuronInput;
// __device__ real *gNeuronInput_I;

// Neuron Tables
// __device__ int *gFiredTable;
// __device__ int *gFiredTableSizes;
__device__ int *gActiveTable;
__device__ int gActiveTableSize;

// Synapse Tables
//__device__ int *gSynapsesActiveTable;
//__device__ int *gSynapsesLogTable;

// Log Arrays
__device__ int *gLayerInput;
__device__ real *gXInput;
__device__ int *gFireCount;

// Connection
// __device__ N2SConnection *gConnection;


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

__global__ void update_time(Connection *connection, int time, int *firedTableSizes)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		// gCurrentCycle = gCurrentCycle + 1;
		// gCurrentIdx = (gCurrentIdx +1)%(MAX_DELAY + 1);
		int currentIdx = time % (connection->maxDelay-connection->minDelay + 1);
		gActiveTableSize = 0;
		firedTableSizes[currentIdx] = 0;
		// gSynapsesActiveTableSize = 0;
	}
	__syncthreads();
}
// 
// __global__ void init_time(int gCurrentCycle)
// {
// 	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
// 		//gTimeTable[gCurrentIdx] = simTime;
// 		gCurrentCycle = gCurrentCycle;
// 		gCurrentIdx = (gCurrentCycle)%(MAX_DELAY + 1);
// 		gActiveTableSize = 0;
// 		gFiredTableSizes[gCurrentIdx] = 0;
// 		gSynapsesActiveTableSize = 0;
// 	}
// 	__syncthreads();
// }

// __global__ void reset_active_synapse()
// {
// 	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
// 		gSynapsesActiveTableSize = 0;
// 	}
// 	__syncthreads();
// 
// }

__global__ void curand_setup_kernel(curandState *state, int num)
{
	int id = threadIdx.x + blockIdx.x * blockDim.x;
	if (id < num) {
		curand_init(1234, id, 0, &state[id]); 
	}
}

__global__ void cudaAddCrossNeurons(Connection *connection, int *firedTable, int *firedTableSizes, int *ids, int num, int time)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delayIdx = time % (connection->maxDelay-connection->minDelay+1);
	if (tid < num) {
		firedTable[gFiredTableCap*delayIdx + firedTableSizes[delayIdx] + tid] = ids[tid];
	}
	__syncthreads();

	if (tid == 0) {
		firedTableSizes[delayIdx] += num;
	}
}

__global__ void cudaDeliverNeurons(Connection *conn, int *firedTable, int *firedTableSizes, int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num, int time)
{
	__shared__ int cross_neuron_id[MAXBLOCKSIZE];
	__shared__ volatile int cross_cnt;

	if (threadIdx.x == 0) {
		cross_cnt = 0;
	}
	__syncthreads();

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int delayIdx = time % (conn->maxDelay-conn->minDelay+1);
	int fired_size = firedTableSizes[delayIdx];
	for (int node = 0; node < node_num; node++) {
		for (int idx = tid; idx < fired_size; idx += blockDim.x * gridDim.x) {
			int nid = firedTable[gFiredTableCap*delayIdx + idx];
			int tmp = idx2index[nid];
			if (tmp >= 0) {
				int map_nid = crossnode_index2idx[tmp*node_num + node];
				if (map_nid >= 0) {
					int test_loc = atomicAdd((int*)&cross_cnt, 1);
					if (test_loc < MAXBLOCKSIZE) {
						cross_neuron_id[test_loc] = map_nid;
					}
				}
			}
			__syncthreads();

			if (cross_cnt > 0) {
				commit2globalTable(cross_neuron_id, cross_cnt, global_cross_data, &fired_n_num[node], gFiredTableCap*node);
				if (threadIdx.x == 0) {
					cross_cnt = 0;
				}
			}
			__syncthreads();
		}
		__syncthreads();
	}
}

// __global__ void init_connection(N2SConnection *pConnection)
// {
// 	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
// 		gConnection = pConnection;
// 	}
// }

BlockSize * getBlockSize(int nSize, int sSize)
{
	BlockSize *ret = (BlockSize*)malloc(sizeof(BlockSize)*TYPESIZE);
	memset(ret, 0, sizeof(BlockSize)*TYPESIZE);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Array].minGridSize), &(ret[Array].blockSize), update_array_neuron, 0, nSize); 
	// ret[Array].gridSize = (upzero_else_set_one(nSize) + (ret[Array].blockSize) - 1) / (ret[Array].blockSize);

	cudaOccupancyMaxPotentialBlockSize(&(ret[LIF].minGridSize), &(ret[LIF].blockSize), update_lif_neuron, 0, nSize); 
	ret[LIF].gridSize = (upzero_else_set_one(nSize) + (ret[LIF].blockSize) - 1) / (ret[LIF].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Constant].minGridSize), &(ret[Constant].blockSize), update_constant_neuron, 0, nSize); 
	// ret[Constant].gridSize = (upzero_else_set_one(nSize) + (ret[Constant].blockSize) - 1) / (ret[Constant].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Poisson].minGridSize), &(ret[Poisson].blockSize), update_poisson_neuron, 0, nSize); 
	// ret[Poisson].gridSize = (upzero_else_set_one(nSize) + (ret[Poisson].blockSize) - 1) / (ret[Poisson].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Decide].minGridSize), &(ret[Decide].blockSize), update_max_neuron, 0, nSize); 
	// ret[Decide].gridSize = (upzero_else_set_one(nSize) + (ret[Decide].blockSize) - 1) / (ret[Decide].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[FFT].minGridSize), &(ret[FFT].blockSize), update_fft_neuron, 0, nSize); 
	// ret[FFT].gridSize = (upzero_else_set_one(nSize) + (ret[FFT].blockSize) - 1) / (ret[FFT].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Mem].minGridSize), &(ret[Mem].blockSize), update_mem_neuron, 0, nSize); 
	// ret[Mem].gridSize = (upzero_else_set_one(nSize) + (ret[Mem].blockSize) - 1) / (ret[Mem].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Max].minGridSize), &(ret[Max].blockSize), update_max_neuron, 0, nSize); 
	// ret[Max].gridSize = (upzero_else_set_one(nSize) + (ret[Max].blockSize) - 1) / (ret[Max].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[TJ].minGridSize), &(ret[TJ].blockSize), update_tj_neuron, 0, nSize); 
	// ret[TJ].gridSize = (upzero_else_set_one(nSize) + (ret[TJ].blockSize) - 1) / (ret[TJ].blockSize);

	// cudaOccupancyMaxPotentialBlockSize(&(ret[Static].minGridSize), &(ret[Static].blockSize), update_static_hit, 0, sSize); 
	ret[Static].blockSize = 128;
	ret[Static].gridSize = (upzero_else_set_one(nSize) + (ret[Static].blockSize) - 1) / (ret[Static].blockSize);

	return ret;
}
