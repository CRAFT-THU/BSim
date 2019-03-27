/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <iostream>

#include "../utils/utils.h"
#include "../utils/FileOp.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
// #include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/GBuffers.h"
#include "../gpu_utils/runtime.h"
#include "../net/MultiNetwork.h"
// #include "../gpu_utils/gpu_func.h"

#include "SingleGPUSimulator.h"

using std::cout;
using std::endl;

CrossNodeDataGPU * gCrossDataNet;

SingleGPUSimulator::SingleGPUSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

SingleGPUSimulator::~SingleGPUSimulator()
{
}

int SingleGPUSimulator::run(real time, FireInfo &log)
{

	int sim_cycle = round(time/dt);

	reset();

	GNetwork *pCPUNet = network->buildNetwork();

	FILE *v_file = openFile("v.data", "w+");

	FILE *input_e_file = openFile("input_e.data", "w+");

	FILE *input_i_file = openFile("input_i.data", "w+");

	FILE *ie_file = openFile("ie.data", "w+");

	FILE *ii_file = openFile("ii.data", "w+");

	FILE *log_file = openFile("sim.log", "w+");

	//findCudaDevice(0, NULL);
	checkCudaErrors(cudaSetDevice(0));
	GNetwork *c_pGPUNet = copyNetworkToGPU(pCPUNet);

	int nTypeNum = c_pGPUNet->nTypeNum;
	int sTypeNum = c_pGPUNet->sTypeNum;
	int totalNeuronNum = c_pGPUNet->neuronNums[nTypeNum];
	int totalSynapseNum = c_pGPUNet->synapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = c_pGPUNet->MAX_DELAY;
	printf("MAX_DELAY: %d\n", c_pGPUNet->MAX_DELAY);


	GBuffers *buffers = alloc_buffers(totalNeuronNum, totalSynapseNum, MAX_DELAY, dt);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);

#ifdef LOG_DATA
	real *c_vm = hostMalloc<real>(totalNeuronNum);

	int copy_idx = getIndex(c_pGPUNet->nTypes, nTypeNum, LIF);

	GLIFNeurons *c_g_lif = copyFromGPU<GLIFNeurons>(static_cast<GLIFNeurons*>(c_pGPUNet->pNeurons[copy_idx]), 1);

	real *c_g_vm = c_g_lif->p_vm;
#ifdef DEBUG 
	real *c_g_ie = c_g_lif->p_i_E;
	real *c_g_ii = c_g_lif->p_i_I;
#endif
#endif

	//real *c_I_syn = hostMalloc<real>(totalSynapseNum);
	//int exp_idx = getIndex(c_pGPUNet->sTypes, sTypeNum, Exp);
	//GExpSynapses *c_g_exp = copyFromGPU<GExpSynapses>(static_cast<GExpSynapses*>(c_pGPUNet->pSynapses[exp_idx]), 1);
	//real *c_g_I_syn = c_g_exp->p_I_syn;

	//for (int i=0; i<nTypeNum; i++) {
	//	cout << c_pGPUNet->nTypes[i] << ": <<<" << updateSize[c_pGPUNet->nTypes[i]].gridSize << ", " << updateSize[c_pGPUNet->nTypes[i]].blockSize << ">>>" << endl;
	//}
	//for (int i=0; i<sTypeNum; i++) {
	//	cout << c_pGPUNet->sTypes[i] << ": <<<" << updateSize[c_pGPUNet->sTypes[i]].gridSize << ", " << updateSize[c_pGPUNet->sTypes[i]].blockSize << ">>>" << endl;
	//}

	vector<int> firedInfo;
	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int time=0; time<sim_cycle; time++) {
		//printf("Cycle: %d ", time);
		//fflush(stdout);

#ifdef DEBUG
		copyFromGPU<real>(c_vm, buffers->c_gNeuronInput + c_pGPUNet->neuronNums[copy_idx], c_pGPUNet->neuronNums[copy_idx+1]-c_pGPUNet->neuronNums[copy_idx]);
		for (int i=0; i<c_pGPUNet->neuronNums[copy_idx+1] - c_pGPUNet->neuronNums[copy_idx]; i++) {
			fprintf(input_e_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(input_e_file, "\n");
		copyFromGPU<real>(c_vm, buffers->c_gNeuronInput_I + c_pGPUNet->neuronNums[copy_idx], c_pGPUNet->neuronNums[copy_idx+1]-c_pGPUNet->neuronNums[copy_idx]);
		for (int i=0; i<c_pGPUNet->neuronNums[copy_idx+1] - c_pGPUNet->neuronNums[copy_idx]; i++) {
			fprintf(input_i_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(input_i_file, "\n");
#endif

		update_time<<<1, 1>>>(time, buffers->c_gFiredTableSizes);

		for (int i=0; i<nTypeNum; i++) {
			cudaUpdateNeuron[c_pGPUNet->nTypes[i]](c_pGPUNet->pNeurons[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGPUNet->neuronNums[i+1]-c_pGPUNet->neuronNums[i], c_pGPUNet->neuronNums[i],time, &updateSize[c_pGPUNet->nTypes[i]]);
		}

		for (int i=0; i<sTypeNum; i++) {
			cudaUpdateSynapse[c_pGPUNet->sTypes[i]](c_pGPUNet->pN2SConnection, c_pGPUNet->pSynapses[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGPUNet->synapseNums[i+1]-c_pGPUNet->synapseNums[i], c_pGPUNet->synapseNums[i], time, &updateSize[c_pGPUNet->sTypes[i]]);
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
		//copyFromGPU<real>(c_I_syn, c_g_I_syn, c_pGPUNet->synapseNums[exp_idx+1]-c_pGPUNet->synapseNums[exp_idx]);

		//fprintf(dataFile, "Cycle %d: ", time);
		copyFromGPU<real>(c_vm, c_g_vm, c_pGPUNet->neuronNums[copy_idx+1]-c_pGPUNet->neuronNums[copy_idx]);
		for (int i=0; i<c_pGPUNet->neuronNums[copy_idx+1] - c_pGPUNet->neuronNums[copy_idx]; i++) {
			fprintf(v_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(v_file, "\n");
#ifdef DEBUG
		copyFromGPU<real>(c_vm, c_g_ie, c_pGPUNet->neuronNums[copy_idx+1]-c_pGPUNet->neuronNums[copy_idx]);
		for (int i=0; i<c_pGPUNet->neuronNums[copy_idx+1] - c_pGPUNet->neuronNums[copy_idx]; i++) {
			fprintf(ie_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(ie_file, "\n");
		copyFromGPU<real>(c_vm, c_g_ii, c_pGPUNet->neuronNums[copy_idx+1]-c_pGPUNet->neuronNums[copy_idx]);
		for (int i=0; i<c_pGPUNet->neuronNums[copy_idx+1] - c_pGPUNet->neuronNums[copy_idx]; i++) {
			fprintf(ii_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(ii_file, "\n");
		//for (int i=0; i<c_pGPUNet->synapseNums[1] - c_pGPUNet->synapseNums[0]; i++) {
		//		fprintf(dataFile, ", %lf", c_I_syn[i]);
		//}
#endif

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

	printf("Simulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	//CALC Firing Rate
	if (log.find("count") != log.end()) {
		int *rate = (int*)malloc(sizeof(int)*totalNeuronNum);
		copyFromGPU<int>(rate, buffers->c_gFireCount, totalNeuronNum);

		log["count"].size = totalNeuronNum;
		log["count"].data = rate;

		//FILE *rateFile = fopen("GFire.log", "w+");
		//if (rateFile == NULL) {
		//	printf("ERROR: Open file Sim.log failed\n");
		//	return -1;
		//}

		//for (int i=0; i<totalNeuronNum; i++) {
		//	fprintf(rateFile, "%d \t", rate[i]);
		//}

		//fflush(rateFile);
		//fclose(rateFile);
	}

	if (log.find("Y") != log.end()) {
		real *Y = (real*)malloc(sizeof(real)*totalNeuronNum);
		copyFromGPU<real>(Y, buffers->c_gXInput, totalNeuronNum);
		log["Y"].size = totalNeuronNum;
		log["Y"].data = Y;
	}

	if (log.find("X") != log.end()) {
		int *X = (int*)malloc(sizeof(int)*totalNeuronNum);
		copyFromGPU<int>(X, buffers->c_gLayerInput, totalNeuronNum);
		log["X"].size = totalNeuronNum;
		log["X"].data = X;
	}



	closeFile(v_file);
	closeFile(input_e_file);
	closeFile(input_i_file);
	closeFile(ie_file);
	closeFile(ii_file);
	closeFile(log_file);

	free_buffers(buffers);
	freeGPUNetwork(c_pGPUNet);
	freeGNetwork(pCPUNet);

	return 0;
}


int SingleGPUSimulator::runMultiNets(real time, int parts, FireInfo &log) {
	int sim_cycle = round(time/dt);
	reset();

	checkCudaErrors(cudaSetDevice(0));

	MultiNetwork multiNet(network, parts);
	DistriNetwork *subnets = multiNet.buildNetworks();
	assert(subnets != NULL);
	CrossNodeDataGPU *crossData = multiNet.arrangeCrossNodeDataGPU(parts);
	assert(crossData != NULL);

	GNetwork ** networks = (GNetwork **)malloc(sizeof(GNetwork *) * parts);
	GBuffers **buffers = (GBuffers **)malloc(sizeof(GBuffers *) * parts);
	BlockSize **updateSizes = (BlockSize **)malloc(sizeof(GBuffers *) * parts);

	for (int i=0; i<parts; i++) {
		subnets[i]._sim_cycle = sim_cycle;
		subnets[i]._node_idx = i;
		subnets[i]._node_num = parts;
		subnets[i]._dt = dt;

		GNetwork *pCPUNet = subnets[i]._network;
		networks[i] = copyNetworkToGPU(pCPUNet);
		GNetwork *c_pGPUNet = networks[i];

		int nTypeNum = c_pGPUNet->nTypeNum;
		int sTypeNum = c_pGPUNet->sTypeNum;
		int nodeNeuronNum = c_pGPUNet->neuronNums[nTypeNum];
		int allNeuronNum = pCPUNet->pN2SConnection->n_num;
		int nodeSynapseNum = c_pGPUNet->synapseNums[sTypeNum];

		buffers[i] = alloc_buffers(allNeuronNum, nodeSynapseNum, c_pGPUNet->MAX_DELAY, dt);
		updateSizes[i] = getBlockSize(allNeuronNum, nodeSynapseNum);

		printf("Subnet %d NeuronTypeNum: %d, SynapseTypeNum: %d\n", subnets[i]._node_idx, nTypeNum, sTypeNum);
		printf("Subnet %d NeuronNum: %d, SynapseNum: %d\n", subnets[i]._node_idx, nodeNeuronNum, nodeSynapseNum);
	}

	for (int time=0; time<sim_cycle; time++) {

		for (int p=0; p<parts; p++) {
			update_time<<<1, 1>>>(time, buffers[p]->c_gFiredTableSizes);

			GNetwork *c_pGPUNet = networks[p];
			for (int i=0; i<c_pGPUNet->nTypeNum; i++) {
				assert(c_pGPUNet->neuronNums[i+1]-c_pGPUNet->neuronNums[i] > 0);
				cudaUpdateNeuron[c_pGPUNet->nTypes[i]](c_pGPUNet->pNeurons[i], buffers[p]->c_gNeuronInput, buffers[p]->c_gNeuronInput_I, buffers[p]->c_gFiredTable, buffers[p]->c_gFiredTableSizes, c_pGPUNet->neuronNums[i+1]-c_pGPUNet->neuronNums[i], c_pGPUNet->neuronNums[i], time, &updateSizes[p][c_pGPUNet->nTypes[i]]);
			}
		}

		for (int p=0; p<parts; p++) {
			GNetwork *c_pGPUNet = networks[p];
			for (int i=0; i<c_pGPUNet->sTypeNum; i++) {
				assert(c_pGPUNet->synapseNums[i+1]-c_pGPUNet->synapseNums[i] > 0);
				cudaUpdateSynapse[c_pGPUNet->sTypes[i]](c_pGPUNet->pN2SConnection, c_pGPUNet->pSynapses[i], buffers[p]->c_gNeuronInput, buffers[p]->c_gNeuronInput_I, buffers[p]->c_gFiredTable, buffers[p]->c_gFiredTableSizes, c_pGPUNet->synapseNums[i+1]-c_pGPUNet->synapseNums[i], c_pGPUNet->synapseNums[i], time, &updateSizes[p][c_pGPUNet->sTypes[i]]);
			}
		}
	}

	for (int i=0; i<parts; i++) {
		freeGPUNetwork(networks[i]);
		free_buffers(buffers[i]);
	}

	free(networks);
	free(buffers);
	free(updateSizes);

	return 0;
}

