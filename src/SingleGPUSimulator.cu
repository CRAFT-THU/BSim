/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <stdio.h>

#include "utils/cuda/helper_cuda.h"
#include "gpu_kernel.h"
#include "SingleGPUSimulator.h"

SingleGPUSimulator::SingleGPUSimulator(Network *network, real dt)
	: SimulatorBase(network, dt)
{
}

SingleGPUSimulator::~SingleGPUSimulator()
{
}

int SingleGPUSimulator::run(real time)
{
	int sim_cycle = (int) (time/dt);

	reset();

	PlainNetwork *pCpuNet = network->buildNetwrok();
	PlainNetwork *c_pGpuNet = copyDataToGPU(pCpuNet);

	unsigned int *c_gTimeTable = NULL;
	unsigned int *c_gFiredTable = NULL;
	bool *c_gSynapsesFiredTable = NULL;

	GLIFNeurons *pN = (GLIFNeurons*)pCpuNet->pNeurons;
	GExpSynapses *pS = (GExpSynapses*)pCpuNet->pSynapses;
	checkCudaErrors(cudaMalloc((void**)c_gTimeTable, sizeof(unsigned int)*(1000+pCpuNet->MAX_DELAY)));
	//TODO: need to adapt the length
	checkCudaErrors(cudaMalloc((void**)c_gFiredTable, sizeof(unsigned int)*((pN->num)*(pCpuNet->MAX_DELAY)*1000)));
	checkCudaErrors(cudaMalloc((void**)c_gSynapsesFiredTable, sizeof(bool)*(pS->num)));

	init_global<<<1, 1>>>(c_pGpuNet, c_gTimeTable, c_gFiredTable, c_gSynapsesFiredTable);

	printf("Start runing for %d cycles\n", sim_cycle);
	for (int i=0; i<sim_cycle; i++) {
		printf("\rCycle: %d", i);
		fflush(stdout);

	}
	printf("\nFinish runing\n");

	return 0;
}

PlainNetwork* SingleGPUSimulator::copyDataToGPU(PlainNetwork *pCpuNet)
{
	PlainNetwork * ret = NULL;

	GLIFNeurons *pN = (GLIFNeurons*)pCpuNet->pNeurons;
	GExpSynapses *pS = (GExpSynapses*)pCpuNet->pSynapses;

	checkCudaErrors(cudaMalloc((void**)ret, sizeof(PlainNetwork)));
	checkCudaErrors(cudaMemcpy(ret, pCpuNet, sizeof(PlainNetwork), cudaMemcpyHostToDevice));
	
	checkCudaErrors(cudaMalloc((void**)ret->pNeurons, sizeof(GLIFNeurons)));
	checkCudaErrors(cudaMemcpy(ret->pNeurons, pCpuNet->pNeurons, sizeof(GLIFNeurons), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)ret->pSynapses, sizeof(GExpSynapses)));
	checkCudaErrors(cudaMemcpy(ret->pSynapses, pCpuNet->pSynapses, sizeof(GExpSynapses), cudaMemcpyHostToDevice));

	pN->allocGNeurons((GLIFNeurons*)ret->pNeurons);
	pS->allocGSynapses((GExpSynapses*)ret->pSynapses);

	return ret;
}
