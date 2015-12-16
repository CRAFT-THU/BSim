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

PlainNetwork* copyDataToGPU(PlainNetwork *);
int freeGPUData(PlainNetwork *);

int SingleGPUSimulator::run(real time)
{
	findCudaDevice(0, NULL);
	FILE *fileN = fopen("GNeurons.txt", "w+");
	FILE *fileS = fopen("GSynapses.txt", "w+");

	int sim_cycle = (int) (time/dt);

	reset();

	PlainNetwork *pCpuNet = network->buildNetwrok();
	real * c_n_vm = (real*)malloc(sizeof(real)*pCpuNet->neuronNum);
	real * c_s_vm = (real*)malloc(sizeof(real)*pCpuNet->synapseNum);

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
	for (unsigned int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);
		update_lif_neuron<<<1, 1>>>((GLIFNeurons*)c_pGpuNet->pNeurons, c_pGpuNet->neuronNum, time);
		update_postSynapse<<<1,1>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_exp_synapse<<<1,1>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);

		checkCudaErrors(cudaMemcpy(c_n_vm, ((GLIFNeurons*)(c_pGpuNet->pNeurons))->p_vm, sizeof(real)*pCpuNet->neuronNum,cudaMemcpyDeviceToHost));
		checkCudaErrors(cudaMemcpy(c_s_vm, ((GExpSynapses*)(c_pGpuNet->pSynapses))->p_I_syn, sizeof(real)*pCpuNet->synapseNum,cudaMemcpyDeviceToHost));
		for (int i=0; i<pCpuNet->neuronNum; i++) {
			fprintf(fileN, "%lf ", c_n_vm[i]); 
		}
		fprintf(fileN, "\n");
		for (int i=0; i<pCpuNet->synapseNum; i++) {
			fprintf(fileS, "%lf ", c_s_vm[i]); 
		}
		fprintf(fileS, "\n");

		fflush(stdout);

	}
	printf("\nFinish runing\n");

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));
	freeGPUData(c_pGpuNet);

	fflush(fileN);
	fflush(fileS);
	fclose(fileN);
	fclose(fileS);

	return 0;
}

PlainNetwork* copyDataToGPU(PlainNetwork *pCpuNet)
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

int freeGPUData(PlainNetwork *pGpuNet)
{
	checkCudaErrors(cudaFree(pGpuNet->pSynapses));
	checkCudaErrors(cudaFree(pGpuNet->pNeurons));
	checkCudaErrors(cudaFree(pGpuNet));

	return 0;
}
