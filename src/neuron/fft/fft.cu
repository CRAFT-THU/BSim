
#include "../../gpu_utils/runtime.h"

#include "GFFTNeurons.h"

__global__ void update_fft_neuron(GFFTNeurons *d_neurons, int num, int start_id)
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
		real res = 0;
		int gnid = idx + start_id;

		if (idx % 2 == 0) {
			res = d_neurons->p_res[idx/2].x;
		} else {
			res = d_neurons->p_res[idx/2].y;
		}

		fired = res > (d_neurons->p_fire_count[idx] + 0.5);

		gFireCount[gnid] += fired;

		int input = gNeuronInput[gnid] + gNeuronInput_I[gnid];
		gXInput[gnid] += input;

		if (idx % 2 == 0) {
			d_neurons->p_input[idx/2].x  += input;
		} else {
			d_neurons->p_input[idx/2].y  += input;
		}

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

		gNeuronInput[gnid] = 0;
		gNeuronInput_I[gnid] = 0;

		__syncthreads();
	}
	__syncthreads();

	if (fire_cnt > 0) {
		commit2globalTable(fire_table_t, fire_cnt, gFiredTable, &(gFiredTableSizes[gCurrentIdx]), gFiredTableCap*gCurrentIdx);
		if (threadIdx.x == 0) {
			fire_cnt = 0;
		}
	}
}

int cudaUpdateFFT(void *data, int num, int start_id, BlockSize *pSize)
{
	GFFTNeurons * p = (GFFTNeurons*)data;
	GFFTNeurons tmp;
	cudaMemcpy(&tmp, p, sizeof(GFFTNeurons), cudaMemcpyDeviceToHost);

	cufftHandle plan;
	cufftPlan1d(&plan, num/2, CUFFT_C2C, 1);
	cufftExecC2C(plan, tmp.p_input, tmp.p_res, CUFFT_FORWARD);

	update_fft_neuron<<<pSize->gridSize, pSize->blockSize>>>((GFFTNeurons*)data, num, start_id);

	return 0;
}

