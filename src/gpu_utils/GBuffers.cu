
#include "../third_party/cuda/helper_cuda.h"
#include "mem_op.h"
// #include "gpu_macros.h"
#include "runtime.h"

#include "GBuffers.h"

__global__ void init_buffers(/*int *c_gTimeTable, real *c_gNeuronInput, real *c_gNeuronInput_I, int *c_gFiredTable, int *c_gFiredTableSizes, */int *c_gActiveTable/*, int *c_gSynapsesActiveTable, int *c_gSynapsesLogTable*/) 
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		// gCurrentIdx = 0;
		// gCurrentCycle = 0;

		// gTimeTable = c_gTimeTable;
		// gNeuronInput = c_gNeuronInput;
		// gNeuronInput_I = c_gNeuronInput_I;
		// gFiredTable = c_gFiredTable;
		// gFiredTableSize = 0;
		// gFiredTableSizes = c_gFiredTableSizes;
		gActiveTable = c_gActiveTable;
		gActiveTableSize = 0;
		// gSynapsesActiveTable = c_gSynapsesActiveTable;
		// gSynapsesActiveTableSize = 0;
		// gSynapsesLogTable = c_gSynapsesLogTable;
	}
}

__global__ void init_log_buffers(int *layer_input, real *x_input, int *fire_count)
{
	if ((threadIdx.x == 0) && (blockIdx.x == 0)) {
		gLayerInput = layer_input;
		gXInput = x_input;
		gFireCount = fire_count;
	}
}


GBuffers* alloc_buffers(int neuron_num, int synapse_num, int max_delay, real dt) 
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

	// checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesActiveTable), sizeof(int)*(synapse_num)));
	// checkCudaErrors(cudaMemset(ret->c_gSynapsesActiveTable, 0, sizeof(int)*(synapse_num)));

	// checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesLogTable), sizeof(int)*(synapse_num)));
	// checkCudaErrors(cudaMemset(ret->c_gSynapsesLogTable, 0, sizeof(int)*(synapse_num)));

	ret->c_gLayerInput = gpuMalloc<int>(neuron_num);
	ret->c_gXInput = gpuMalloc<real>(neuron_num);
	ret->c_gFireCount = gpuMalloc<int>(neuron_num);

	// int timeTableCap = max_delay+1;
	checkCudaErrors(cudaMemcpyToSymbol(MAX_DELAY, &max_delay, sizeof(int)));
	// checkCudaErrors(cudaMemcpyToSymbol(gTimeTableCap, &timeTableCap, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gFiredTableCap, &neuron_num, sizeof(int)));
	// checkCudaErrors(cudaMemcpyToSymbol(gSynapsesTableCap, &synapse_num, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(DT, &dt, sizeof(real)));
	//checkCudaErrors(cudaMalloc((void**)&ret->c_gTimeTable, sizeof(int)*(max_delay+1)));
	//checkCudaErrors(cudaMemset(ret->c_gTimeTable, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMallocHost((void**)(&ret->c_neuronsFired), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMallocHost((void**)(&ret->c_synapsesFired), sizeof(int)*(synapse_num)));

	// init_buffers<<<1, 1, 0>>>(/*ret->c_gTimeTable,*/ ret->c_gNeuronInput, ret->c_gNeuronInput_I, ret->c_gFiredTable, ret->c_gFiredTableSizes, ret->c_gActiveTable/*, ret->c_gSynapsesActiveTable, ret->c_gSynapsesLogTable*/);
	init_buffers<<<1, 1, 0>>>(ret->c_gActiveTable);

	init_log_buffers<<<1, 1, 0>>>(ret->c_gLayerInput, ret->c_gXInput, ret->c_gFireCount);

	return ret;
}

// void init_buffers(GBuffers * buf) {
// 	init_buffers<<<1, 1, 0>>>(/*buf->c_gTimeTable,*/ buf->c_gNeuronInput, buf->c_gNeuronInput_I, buf->c_gFiredTable, buf->c_gFiredTableSizes, buf->c_gActiveTable/*, buf->c_gSynapsesActiveTable, buf->c_gSynapsesLogTable*/);
// 
// 	init_log_buffers<<<1, 1, 0>>>(buf->c_gLayerInput, buf->c_gXInput, buf->c_gFireCount);
// }

int free_buffers(GBuffers *buf) 
{
	checkCudaErrors(cudaFree(buf->c_gNeuronInput));
	checkCudaErrors(cudaFree(buf->c_gNeuronInput_I));
	checkCudaErrors(cudaFree(buf->c_gFiredTable));
	checkCudaErrors(cudaFree(buf->c_gFiredTableSizes));
	checkCudaErrors(cudaFree(buf->c_gActiveTable));
	// checkCudaErrors(cudaFree(buf->c_gSynapsesActiveTable));
	// checkCudaErrors(cudaFree(buf->c_gSynapsesLogTable));

	checkCudaErrors(cudaFreeHost(buf->c_neuronsFired));
	checkCudaErrors(cudaFreeHost(buf->c_synapsesFired));

	return 0;
}
