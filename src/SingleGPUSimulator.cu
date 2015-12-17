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

	int sim_cycle = (int) (time/dt);

	reset();

	PlainNetwork *pCpuNet = network->buildNetwrok();
	real * c_n_vm = (real*)malloc(sizeof(real)*pCpuNet->neuronNum);
	real * c_s_vm = (real*)malloc(sizeof(real)*pCpuNet->synapseNum);
	FILE **fileN = (FILE**)malloc(sizeof(FILE*)*pCpuNet->neuronNum);
	char filename[128];
	for (int i=0; i<pCpuNet->neuronNum; i++) {
		sprintf(filename, "GNeuron_%d.log", i);
		fileN[i] = fopen(filename, "w+");
	}
	FILE **fileS = (FILE**)malloc(sizeof(FILE*)*pCpuNet->synapseNum);
	for (int i=0; i<pCpuNet->synapseNum; i++) {
		sprintf(filename, "GSynapse_%d.log", i);
		fileS[i] = fopen(filename, "w+");
	}

	PlainNetwork *c_pGpuNet = copyDataToGPU(pCpuNet);
	unsigned int *c_gTimeTable = NULL;
	unsigned int *c_gFiredTable = NULL;
	bool *c_gSynapsesFiredTable = NULL;


	unsigned int MAX_DELAY = (unsigned int)(pCpuNet->MAX_DELAY/dt);
	printf("MAX_DELAY: %lf %lf %lf\n", network->maxDelay, pCpuNet->MAX_DELAY, dt);
	printf("MAX_DELAY: %u\n", MAX_DELAY);

	GLIFNeurons *pN = (GLIFNeurons*)pCpuNet->pNeurons;
	GExpSynapses *pS = (GExpSynapses*)pCpuNet->pSynapses;
	checkCudaErrors(cudaMalloc((void**)&c_gTimeTable, sizeof(unsigned int)*(1000+MAX_DELAY+1)));
	checkCudaErrors(cudaMemset(c_gTimeTable, 0, sizeof(unsigned int)*(1000+MAX_DELAY+1)));
	//TODO: need to adapt the length
	checkCudaErrors(cudaMalloc((void**)&c_gFiredTable, sizeof(unsigned int)*((pN->num)*(MAX_DELAY+1)*1000)));
	checkCudaErrors(cudaMemset(c_gFiredTable, 0, sizeof(unsigned int)*((pN->num)*(MAX_DELAY+1)*1000)));
	checkCudaErrors(cudaMalloc((void**)&c_gSynapsesFiredTable, sizeof(bool)*(pS->num)));
	checkCudaErrors(cudaMemset(c_gSynapsesFiredTable, 0, sizeof(bool)*(pS->num)));

	GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	checkCudaErrors(cudaMemcpy(pNTmp, ((GLIFNeurons*)(c_pGpuNet->pNeurons)), sizeof(GLIFNeurons), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pSTmp, ((GExpSynapses*)(c_pGpuNet->pSynapses)), sizeof(GExpSynapses), cudaMemcpyDeviceToHost));
	real *g_n_vm = pNTmp->p_vm;
	real *g_s_vm = pSTmp->p_I_syn;
	free(pNTmp);
	free(pSTmp);

	init_global<<<1, 1, 0>>>(MAX_DELAY, c_gTimeTable, 1000+MAX_DELAY+1, c_gFiredTable, (pN->num)*(MAX_DELAY+1)*1000, c_gSynapsesFiredTable, pS->num);

	printf("Start runing for %d cycles\n", sim_cycle);
	for (unsigned int time=0; time<sim_cycle; time++) {
		printf("\rCycle: %d", time);
		update_lif_neuron<<<1, 2, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, c_pGpuNet->neuronNum, time);
		update_pre_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, time);
		update_exp_synapse<<<1, 1, 0>>>((GLIFNeurons*)c_pGpuNet->pNeurons, (GExpSynapses*)c_pGpuNet->pSynapses, c_pGpuNet->synapseNum, time);

		checkCudaErrors(cudaMemcpy(c_n_vm, g_n_vm, sizeof(real)*pCpuNet->neuronNum,cudaMemcpyDeviceToHost));
		checkCudaErrors(cudaMemcpy(c_s_vm, g_s_vm, sizeof(real)*pCpuNet->synapseNum,cudaMemcpyDeviceToHost));
		for (int i=0; i<pCpuNet->neuronNum; i++) {
			fprintf(fileN[i], "%lf\n", c_n_vm[i]); 
		}
		for (int i=0; i<pCpuNet->synapseNum; i++) {
			fprintf(fileS[i], "%lf\n", c_s_vm[i]); 
		}

		fflush(stdout);

	}
	printf("\nFinish runing\n");

	checkCudaErrors(cudaFree(c_gTimeTable));
	checkCudaErrors(cudaFree(c_gFiredTable));
	checkCudaErrors(cudaFree(c_gSynapsesFiredTable));
	freeGPUData(c_pGpuNet);

	for (int i=0; i<pCpuNet->neuronNum; i++) {
		fflush(fileN[i]);
		fclose(fileN[i]);
	}
	for (int i=0; i<pCpuNet->synapseNum; i++) {
		fflush(fileS[i]);
		fclose(fileS[i]);
	}

	free(fileN);
	free(fileS);

	return 0;
}

PlainNetwork* copyDataToGPU(PlainNetwork *pCpuNet)
{
	PlainNetwork *tmpNet = (PlainNetwork*)malloc(sizeof(PlainNetwork));
	memcpy(tmpNet, pCpuNet, sizeof(PlainNetwork));

	GLIFNeurons *pN = (GLIFNeurons*)pCpuNet->pNeurons;
	GExpSynapses *pS = (GExpSynapses*)pCpuNet->pSynapses;

	GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	memcpy(pNTmp, pN, sizeof(GLIFNeurons));
	GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	memcpy(pSTmp, pS, sizeof(GExpSynapses));

	GLIFNeurons *pNGpu;
	GExpSynapses *pSGpu;

	pN->allocGNeurons((GLIFNeurons*)pNTmp);
	pS->allocGSynapses((GExpSynapses*)pSTmp);
	
	checkCudaErrors(cudaMalloc((void**)&(pNGpu), sizeof(GLIFNeurons)));
	checkCudaErrors(cudaMemcpy(pNGpu, pNTmp, sizeof(GLIFNeurons), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pSGpu), sizeof(GExpSynapses)));
	checkCudaErrors(cudaMemcpy(pSGpu, pSTmp, sizeof(GExpSynapses), cudaMemcpyHostToDevice));
	free(pNTmp);
	free(pSTmp);

	tmpNet->pNeurons = pNGpu;
	tmpNet->pSynapses = pSGpu;

	return tmpNet;
}


int freeGPUData(PlainNetwork *pGpuNet)
{
	GLIFNeurons *pN = (GLIFNeurons*)pGpuNet->pNeurons;
	GExpSynapses *pS = (GExpSynapses*)pGpuNet->pSynapses;

	GLIFNeurons *pNTmp = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	GExpSynapses *pSTmp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	checkCudaErrors(cudaMemcpy(pNTmp, pN, sizeof(GLIFNeurons), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(pSTmp, pS, sizeof(GExpSynapses), cudaMemcpyDeviceToHost));

	freeGNeurons(pNTmp);
	freeGSynapses(pSTmp);

	free(pNTmp);
	free(pSTmp);

	checkCudaErrors(cudaFree(pN));
	checkCudaErrors(cudaFree(pS));
	free(pGpuNet);

	return 0;
}
