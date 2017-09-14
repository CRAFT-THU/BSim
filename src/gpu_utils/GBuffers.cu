
#include "GBuffers.h"

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

	//checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesActiveTable), sizeof(int)*(synapse_num)));
	//checkCudaErrors(cudaMemset(ret->c_gSynapsesActiveTable, 0, sizeof(int)*(synapse_num)));

	//checkCudaErrors(cudaMalloc((void**)&(ret->c_gSynapsesLogTable), sizeof(int)*(synapse_num)));
	//checkCudaErrors(cudaMemset(ret->c_gSynapsesLogTable, 0, sizeof(int)*(synapse_num)));

	ret->c_gLayerInput = gpuMalloc<int>(neuron_num);
	ret->c_gXInput = gpuMalloc<real>(neuron_num);
	ret->c_gFireCount = gpuMalloc<int>(neuron_num);

	int timeTableCap = max_delay+1;
	checkCudaErrors(cudaMemcpyToSymbol(MAX_DELAY, &max_delay, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gTimeTableCap, &timeTableCap, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gFiredTableCap, &neuron_num, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(gSynapsesTableCap, &synapse_num, sizeof(int)));
	checkCudaErrors(cudaMemcpyToSymbol(DT, &dt, sizeof(real)));
	//checkCudaErrors(cudaMalloc((void**)&ret->c_gTimeTable, sizeof(int)*(max_delay+1)));
	//checkCudaErrors(cudaMemset(ret->c_gTimeTable, 0, sizeof(int)*(max_delay+1)));

	checkCudaErrors(cudaMallocHost((void**)(&ret->c_neuronsFired), sizeof(int)*(neuron_num)));
	checkCudaErrors(cudaMallocHost((void**)(&ret->c_synapsesFired), sizeof(int)*(synapse_num)));

	init_buffers<<<1, 1, 0>>>(/*ret->c_gTimeTable,*/ ret->c_gNeuronInput, ret->c_gNeuronInput_I, ret->c_gFiredTable, ret->c_gFiredTableSizes, ret->c_gActiveTable, ret->c_gSynapsesActiveTable, ret->c_gSynapsesLogTable);

	init_log_buffers<<<1, 1, 0>>>(ret->c_gLayerInput, ret->c_gXInput, ret->c_gFireCount);

	return ret;
}

void init_buffers(GBuffers * buf) {
	init_buffers<<<1, 1, 0>>>(/*buf->c_gTimeTable,*/ buf->c_gNeuronInput, buf->c_gNeuronInput_I, buf->c_gFiredTable, buf->c_gFiredTableSizes, buf->c_gActiveTable, buf->c_gSynapsesActiveTable, buf->c_gSynapsesLogTable);

	init_log_buffers<<<1, 1, 0>>>(buf->c_gLayerInput, buf->c_gXInput, buf->c_gFireCount);
}

int free_buffers(GBuffers *buf) 
{
	checkCudaErrors(cudaFree(buf->c_gNeuronInput));
	checkCudaErrors(cudaFree(buf->c_gNeuronInput_I));
	checkCudaErrors(cudaFree(buf->c_gFiredTable));
	checkCudaErrors(cudaFree(buf->c_gFiredTableSizes));
	checkCudaErrors(cudaFree(buf->c_gActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesActiveTable));
	checkCudaErrors(cudaFree(buf->c_gSynapsesLogTable));

	checkCudaErrors(cudaFreeHost(buf->c_neuronsFired));
	checkCudaErrors(cudaFreeHost(buf->c_synapsesFired));

	return 0;
}
