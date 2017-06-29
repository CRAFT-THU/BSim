/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <iostream>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
#include "../gpu_utils/gpu_func.h"
#include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/gpu_kernel.h"
#include "SingleGPUSimulator.h"

using std::cout;
using std::endl;

SingleGPUSimulator::SingleGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

SingleGPUSimulator::~SingleGPUSimulator()
{
}

int SingleGPUSimulator::run(real time)
{

	int sim_cycle = round(time/dt);

	reset();

	GNetwork *pCpuNet = network->buildNetwork();

	FILE *v_file = fopen("g_v.data", "w+");
	if (v_file == NULL) {
		printf("ERROR: Open file GSim.data failed\n");
		return -1;
	}

	FILE *input_e_file = fopen("g_input_e.data", "w+");
	if (input_e_file == NULL) {
		printf("Open file input_e.data failed\n");
		return -1;
	}

	FILE *input_i_file = fopen("g_input_i.data", "w+");
	if (input_i_file == NULL) {
		printf("Open file input_i.data failed\n");
		return -1;
	}

	FILE *ie_file = fopen("g_ie.data", "w+");
	if (ie_file == NULL) {
		printf("ERROR: Open file GSim.data failed\n");
		return -1;
	}

	FILE *ii_file = fopen("g_ii.data", "w+");
	if (ii_file == NULL) {
		printf("ERROR: Open file GSim.data failed\n");
		return -1;
	}

	FILE *log_file = fopen("GSim.log", "w+");
	if (log_file == NULL) {
		printf("ERROR: Open file GSim.log failed\n");
		return -1;
	}

	findCudaDevice(0, NULL);
	GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = pCpuNet->MAX_DELAY;
	printf("MAX_DELAY: %d\n", pCpuNet->MAX_DELAY);

	init_connection<<<1, 1>>>(c_pGpuNet->pN2SConnection);

	GBuffers *buffers = alloc_buffers(totalNeuronNum, totalSynapseNum, MAX_DELAY);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
	//BlockSize preSize = { 0, 0, 0};
	//cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_pre_synapse, 0, totalNeuronNum); 
	//preSize.gridSize = (totalSynapseNum + (preSize.blockSize) - 1) / (preSize.blockSize);

#ifdef LOG_DATA
	real *c_vm = hostMalloc<real>(totalNeuronNum);

	int tj_idx = getIndex(pCpuNet->nTypes, nTypeNum, TJ);
	int life_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIFE);
	int lif_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIF);
	int copy_idx = -1;
	real *c_g_vm = NULL;
	real *c_g_ie = NULL;
	real *c_g_ii = NULL;

	if (life_idx >= 0) {
		GLIFENeurons *c_g_lif = copyFromGPU<GLIFENeurons>(static_cast<GLIFENeurons*>(c_pGpuNet->pNeurons[life_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
		c_g_ie = c_g_lif->p_i_E;
		c_g_ii = c_g_lif->p_i_I;
		copy_idx = life_idx;
	} else if (lif_idx >= 0) {
		GLIFNeurons *c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGpuNet->pNeurons[lif_idx]), 1);
		c_g_vm = c_g_lif->p_vm;
		copy_idx = lif_idx;
	} else if (tj_idx >= 0) {
		GTJNeurons *c_g_tj = copyFromGPU<GTJNeurons>(static_cast<GTJNeurons*>(c_pGpuNet->pNeurons[tj_idx]), 1);
		c_g_vm = c_g_tj->p_vm;
		copy_idx = tj_idx;

	} else {
	}
#endif

	//real *c_I_syn = hostMalloc<real>(totalSynapseNum);
	//int exp_idx = getIndex(pCpuNet->sTypes, sTypeNum, Exp);
	//GExpSynapses *c_g_exp = copyFromGPU<GExpSynapses>(static_cast<GExpSynapses*>(c_pGpuNet->pSynapses[exp_idx]), 1);
	//real *c_g_I_syn = c_g_exp->p_I_syn;

	for (int i=0; i<nTypeNum; i++) {
		cout << pCpuNet->nTypes[i] << ": <<<" << updateSize[c_pGpuNet->nTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->nTypes[i]].blockSize << ">>>" << endl;
	}
	for (int i=0; i<sTypeNum; i++) {
		cout << pCpuNet->sTypes[i] << ": <<<" << updateSize[c_pGpuNet->sTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->sTypes[i]].blockSize << ">>>" << endl;
	}

	vector<int> firedInfo;
	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<sim_cycle; time++) {
		//printf("Cycle: %d ", time);
		//fflush(stdout);

#ifdef LOG_DATA
		if (copy_idx >= 0) {
			//copyFromGPU<real>(c_vm, buffers->c_gNeuronInput + c_pGpuNet->neuronNums[copy_idx], c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			copyFromGPU<real>(c_vm, buffers->c_gNeuronInput + c_pGpuNet->neuronNums[copy_idx], c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
				fprintf(input_e_file, "%.10lf \t", c_vm[i]);
			}
			//copyFromGPU<real>(c_vm, buffers->c_gNeuronInput_I + c_pGpuNet->neuronNums[copy_idx], c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			copyFromGPU<real>(c_vm, buffers->c_gNeuronInput_I + c_pGpuNet->neuronNums[copy_idx], c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
				fprintf(input_i_file, "%.10lf \t", c_vm[i]);
			}
		}
#endif

		for (int i=0; i<nTypeNum; i++) {
			cudaUpdateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
		}

		//update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(c_pGpuNet->pN2SConnection);

		for (int i=0; i<sTypeNum; i++) {
			cudaUpdateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], &updateSize[pCpuNet->sTypes[i]]);
		}


#ifdef LOG_DATA
		//LOG DATA
		int currentIdx = time%(MAX_DELAY+1);

		//printf("HERE %p %d ", buffers->c_gFiredTableSizes, currentIdx);
		//fflush(stdout);
		int copySize = 0;
		copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
		//printf("HERE1\n");
		//fflush(stdout);
		copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (totalNeuronNum*currentIdx), copySize);

		//copyFromGPU<real>(c_I_syn, c_g_I_syn, c_pGpuNet->synapseNums[exp_idx+1]-c_pGpuNet->synapseNums[exp_idx]);

		//fprintf(dataFile, "Cycle %d: ", time);
		if (copy_idx >= 0) {
			copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
			for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
				fprintf(v_file, "%.10lf \t", c_vm[i]);
			}


			if (life_idx >= 0) {
				copyFromGPU<real>(c_vm, c_g_ie, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
				for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
					fprintf(ie_file, "%.10lf \t", c_vm[i]);
				}
				copyFromGPU<real>(c_vm, c_g_ii, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
				for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
					fprintf(ii_file, "%.10lf \t", c_vm[i]);
				}
			}
		}
		//for (int i=0; i<c_pGpuNet->synapseNums[1] - c_pGpuNet->synapseNums[0]; i++) {
		//		fprintf(dataFile, ", %lf", c_I_syn[i]);
		//}
		fprintf(v_file, "\n");
		fprintf(input_e_file, "\n");
		fprintf(input_i_file, "\n");
		fprintf(ie_file, "\n");
		fprintf(ii_file, "\n");

		for (int i=0; i<copySize; i++) {
			fprintf(log_file, "%d ", buffers->c_neuronsFired[i]);
		}
		fprintf(log_file, "\n");

		//LOG SYNAPSE
		//copyFromGPU<int>(buffers->c_synapsesFired, buffers->c_gSynapsesLogTable, totalSynapseNum);
		//int synapseCount = 0;
		//if (time > 0) {
		//	for (int i=0; i<totalSynapseNum; i++) {
		//		if (buffers->c_synapsesFired[i] == time) {
		//			if (synapseCount ==  0) {
		//				if (copySize > 0) {
		//					fprintf(logFile, ", ");
		//				}
		//				fprintf(logFile, "%s", network->idx2sid[i].getInfo().c_str());
		//				synapseCount++;
		//			} else {
		//				fprintf(logFile, ", %s", network->idx2sid[i].getInfo().c_str());
		//			}
		//		}
		//	}
		//	fprintf(logFile, "\n");
		//}
#endif

		update_time<<<1, 1>>>();
	}
	cudaDeviceSynchronize();

	gettimeofday(&te, NULL);
	long seconds = te.tv_sec - ts.tv_sec;
	long hours = seconds/3600;
	seconds = seconds%3600;
	long minutes = seconds/60;
	seconds = seconds%60;
	long uSeconds = te.tv_usec - ts.tv_usec;
	if (uSeconds < 0) {
		uSeconds += 1000000;
		seconds = seconds - 1;
	}

	printf("\nSimulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	//CALC Firing Rate
	int *rate = (int*)malloc(sizeof(int)*totalNeuronNum);
	copyFromGPU<int>(rate, buffers->c_gFireCount, totalNeuronNum);

	FILE *rateFile = fopen("GFire.log", "w+");
	if (rateFile == NULL) {
		printf("Open file Sim.log failed\n");
		return -1;
	}

	for (int i=0; i<totalNeuronNum; i++) {
		fprintf(rateFile, "%d \t", rate[i]);
	}

	fclose(rateFile);

	fclose(v_file);
	fclose(input_e_file);
	fclose(input_i_file);
	fclose(ie_file);
	fclose(ii_file);
	fclose(log_file);

	free_buffers(buffers);
	freeGPUNetwork(c_pGpuNet);

	return 0;
}

//int SingleGPUSimulator::compare_run(real time)
//{
//
//	int sim_cycle = round(time/dt);
//
//	reset();
//
//	GNetwork *pCpuNet = network->buildNetwork();
//
//	FILE *logFile = fopen("GSim_compare.log", "w+");
//	if (logFile == NULL) {
//		printf("ERROR: Open file SimGPU.log failed\n");
//		return -1;
//	}
//	FILE *dataFile = fopen("GSim_compare.data", "w+");
//	if (dataFile == NULL) {
//		printf("ERROR: Open file SimGPU.log failed\n");
//		return -1;
//	}
//
//	findCudaDevice(0, NULL);
//	GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);
//
//	int nTypeNum = pCpuNet->nTypeNum;
//	int sTypeNum = pCpuNet->sTypeNum;
//	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
//	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
//	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
//	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);
//
//	int MAX_DELAY = pCpuNet->MAX_DELAY;
//	printf("MAX_DELAY: %d\n", pCpuNet->MAX_DELAY);
//
//	GBuffers *buffers = alloc_buffers(totalNeuronNum, totalSynapseNum, MAX_DELAY);
//
//	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
//	//BlockSize preSize = { 0, 0, 0};
//	//cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_pre_synapse, 0, totalNeuronNum); 
//	//preSize.gridSize = (totalNeuronNum + (preSize.blockSize) - 1) / (preSize.blockSize);
//
//	real *c_vm = hostMalloc<real>(totalNeuronNum);
//	int lif_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIF);
//
//	GLIFNeurons *c_g_lif = NULL;
//	real *c_g_vm = NULL;
//	if (lif_idx >= 0) {
//		c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGpuNet->pNeurons[lif_idx]), 1);
//		c_g_vm = c_g_lif->p_vm;
//	}
//	//real *c_I_syn = hostMalloc<real>(totalSynapseNum);
//	//int exp_idx = getIndex(pCpuNet->sTypes, sTypeNum, Exp);
//	//GExpSynapses *c_g_exp = copyFromGPU<GExpSynapses>(static_cast<GExpSynapses*>(c_pGpuNet->pSynapses[exp_idx]), 1);
//	//real *c_g_I_syn = c_g_exp->p_I_syn;
//
//	vector<int> firedInfo;
//	printf("Start runing for %d cycles\n", sim_cycle);
//	struct timeval ts, te;
//	gettimeofday(&ts, NULL);
//	for (int time=0; time<sim_cycle; time++) {
//		//printf("\rCycle: %d", time);
//		//fflush(stdout);
//
//		for (int i=0; i<nTypeNum; i++) {
//			cudaUpdateAllType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], &updateSize[c_pGpuNet->nTypes[i]]);
//		}
//
//		//update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(c_pGpuNet->pN2SConnection);
//
//		for (int i=0; i<sTypeNum; i++) {
//			cudaUpdateAllType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], &updateSize[pCpuNet->nTypes[i]]);
//		}
//
//
//		int currentIdx = time%(MAX_DELAY+1);
//
//		int copySize = 0;
//		copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
//		copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (totalNeuronNum*currentIdx), copySize);
//		if (lif_idx >= 0) {
//			copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[lif_idx+1]-c_pGpuNet->neuronNums[lif_idx]);
//		}
//		//copyFromGPU<real>(c_I_syn, c_g_I_syn, c_pGpuNet->synapseNums[exp_idx+1]-c_pGpuNet->synapseNums[exp_idx]);
//
//		fprintf(logFile, "Cycle %d: ", time);
//		for (int i=0; i<copySize; i++) {
//			//assert(network->idx2nid.find(buffers->c_neuronsFired[i]) != network->idx2nid.end());
//			//printf("%s ", network->idx2nid[buffers->c_neuronsFired[i]].getInfo().c_str());
//			fprintf(logFile, "%d ", buffers->c_neuronsFired[i]);
//		}
//		fprintf(logFile, "\n");
//
//		//fprintf(dataFile, "Cycle %d: ", time);
//		if (lif_idx >= 0) {
//			for (int i=0; i<c_pGpuNet->neuronNums[lif_idx+1] - c_pGpuNet->neuronNums[lif_idx]; i++) {
//				fprintf(dataFile, "%lf ", c_vm[i]);
//			}
//		}
//		//for (int i=0; i<c_pGpuNet->synapseNums[1] - c_pGpuNet->synapseNums[0]; i++) {
//		//		fprintf(dataFile, ", %lf", c_I_syn[i]);
//		//}
//		fprintf(dataFile, "\n");
//
//		//copyFromGPU<int>(buffers->c_synapsesFired, buffers->c_gSynapsesLogTable, totalSynapseNum);
//		//int synapseCount = 0;
//		//if (time > 0) {
//		//	for (int i=0; i<totalSynapseNum; i++) {
//		//		if (buffers->c_synapsesFired[i] == time) {
//		//			if (synapseCount ==  0) {
//		//				if (copySize > 0) {
//		//					fprintf(logFile, ", ");
//		//				}
//		//				fprintf(logFile, "%s", network->idx2sid[i].getInfo().c_str());
//		//				synapseCount++;
//		//			} else {
//		//				fprintf(logFile, ", %s", network->idx2sid[i].getInfo().c_str());
//		//			}
//		//		}
//		//	}
//		//	fprintf(logFile, "\n");
//		//}
//
//		update_time<<<1, 1>>>();
//	}
//	gettimeofday(&te, NULL);
//	long seconds = te.tv_sec - ts.tv_sec;
//	long hours = seconds/3600;
//	seconds = seconds%3600;
//	long minutes = seconds/60;
//	seconds = seconds%60;
//	long uSeconds = te.tv_usec - ts.tv_usec;
//	if (uSeconds < 0) {
//		uSeconds += 1000000;
//		seconds = seconds - 1;
//	}
//
//	printf("\nSimulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);
//
//	fclose(logFile);
//	fclose(dataFile);
//
//	free_buffers(buffers);
//	freeGPUNetwork(c_pGpuNet);
//
//	return 0;
//}
