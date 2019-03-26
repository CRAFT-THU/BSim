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
#include "../gpu_utils/GBuffers.h"
#include "../gpu_utils/runtime.h"
#include "../net/MultiNetwork.h"

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

	GNetwork *pCpuNet = network->buildNetwork();

	FILE *v_file = fopen("g_v.data", "w+");
	if (v_file == NULL) {
		printf("ERROR: Open file g_v.data failed\n");
		return -1;
	}

	FILE *input_e_file = fopen("g_input_e.data", "w+");
	if (input_e_file == NULL) {
		printf("Open file g_input_e.data failed\n");
		return -1;
	}

	FILE *input_i_file = fopen("g_input_i.data", "w+");
	if (input_i_file == NULL) {
		printf("Open file g_input_i.data failed\n");
		return -1;
	}

	FILE *ie_file = fopen("g_ie.data", "w+");
	if (ie_file == NULL) {
		printf("ERROR: Open file g_ie.data failed\n");
		return -1;
	}

	FILE *ii_file = fopen("g_ii.data", "w+");
	if (ii_file == NULL) {
		printf("ERROR: Open file g_ii.data failed\n");
		return -1;
	}

	FILE *info_file = fopen("GSim.info", "w+");
	if (info_file == NULL) {
		printf("ERROR: Open file GSim.info failed\n");
		return -1;
	}

	FILE *log_file = fopen("GSim.log", "w+");
	if (log_file == NULL) {
		printf("ERROR: Open file GSim.log failed\n");
		return -1;
	}

	//findCudaDevice(0, NULL);
	checkCudaErrors(cudaSetDevice(0));
	GNetwork *c_pGpuNet = copyNetworkToGPU(pCpuNet);

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[sTypeNum];
	fprintf(info_file, "NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	fprintf(info_file, "NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int MAX_DELAY = pCpuNet->MAX_DELAY;
	fprintf(info_file, "MAX_DELAY: %d\n", pCpuNet->MAX_DELAY);

	init_connection<<<1, 1>>>(c_pGpuNet->pN2SConnection);

	GBuffers *buffers = alloc_buffers(totalNeuronNum, totalSynapseNum, MAX_DELAY, dt);

	BlockSize *updateSize = getBlockSize(totalNeuronNum, totalSynapseNum);
	//BlockSize preSize = { 0, 0, 0};
	//cudaOccupancyMaxPotentialBlockSize(&(preSize.minGridSize), &(preSize.blockSize), update_pre_synapse, 0, totalNeuronNum); 
	//preSize.gridSize = (totalSynapseNum + (preSize.blockSize) - 1) / (preSize.blockSize);

#ifdef LOG_DATA
	real *c_vm = hostMalloc<real>(totalNeuronNum);

	int tj_idx = getIndex(pCpuNet->nTypes, nTypeNum, TJ);
	int life_idx = getIndex(pCpuNet->nTypes, nTypeNum, LIFE);
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

	//for (int i=0; i<nTypeNum; i++) {
	//	cout << pCpuNet->nTypes[i] << ": <<<" << updateSize[c_pGpuNet->nTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->nTypes[i]].blockSize << ">>>" << endl;
	//}
	//for (int i=0; i<sTypeNum; i++) {
	//	cout << pCpuNet->sTypes[i] << ": <<<" << updateSize[c_pGpuNet->sTypes[i]].gridSize << ", " << updateSize[c_pGpuNet->sTypes[i]].blockSize << ">>>" << endl;
	//}

	vector<int> firedInfo;
	fprintf(info_file, "Start runing for %d cycles\n", sim_cycle);
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

		update_time<<<1, 1>>>(time);

		for (int i=0; i<nTypeNum; i++) {
			cudaUpdateType[pCpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i],time, &updateSize[c_pGpuNet->nTypes[i]]);
		}

		//update_pre_synapse<<<preSize.gridSize, preSize.blockSize>>>(c_pGpuNet->pN2SConnection);

		for (int i=0; i<sTypeNum; i++) {
			cudaUpdateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], buffers->c_gNeuronInput, buffers->c_gNeuronInput_I, buffers->c_gFiredTable, buffers->c_gFiredTableSizes, c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], time, &updateSize[pCpuNet->sTypes[i]]);
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

	fprintf(info_file, "Simulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

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



	fclose(v_file);
	fclose(input_e_file);
	fclose(input_i_file);
	fclose(ie_file);
	fclose(ii_file);
	fclose(info_file);
	fclose(log_file);

	free_buffers(buffers);
	freeGPUNetwork(c_pGpuNet);
	freeGNetwork(pCpuNet);

	return 0;
}


int SingleGPUSimulator::runMultiNets(real time, int parts, FireInfo &log) {
	// int sim_cycle = round(time/dt);
	// reset();

	// checkCudaErrors(cudaSetDevice(0));

	// MultiNetwork multiNet(network, parts);
	// DistriNetwork *node_nets = multiNet.buildNetworks();
	// assert(node_nets != NULL);
	// CrossNodeDataGPU *CrossData = multiNet.arrangeCrossNodeDataGPU(parts);
	// assert(gCrossDataNet != NULL);

	// GNetwork ** networks = (GNetwork **)malloc(sizeof(GNetwork *) * parts);

	// for (int i=0; i<parts; i++) {
	// 	node_nets[i]._sim_cycle = sim_cycle;
	// 	node_nets[i]._node_idx = i;
	// 	node_nets[i]._node_num = parts;
	// 	node_nets[i]._dt = dt;

	// 	DistriNetwork *network = &(node_nets[i]);
	// 	networks[i] = copyNetworkToGPU(network->_network);
	// 	int nTypeNum = network->_network->nTypeNum;
	// 	int sTypeNum = network->_network->sTypeNum;
	// 	printf("Subnet %d NeuronTypeNum: %d, SynapseTypeNum: %d\n", network->_node_idx, nTypeNum, sTypeNum);
	// 	printf("Subnet %d NeuronNum: %d, SynapseNum: %d\n", network->_node_idx, network->_network->neuronNums[nTypeNum], network->_network->synapseNums[sTypeNum]);
	// }

	// for (int time=0; time<sim_cycle; time++) {
	// 	update_time<<<1, 1>>>(time);

	// 	for (int p=0; p<parts; p++) {
	// 		GNetwork * c_pGpuNet = networks[p]; 
	// 		for (int i=0; i<c_pGpuNet->nTypeNum; i++) {
	// 			assert(c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i] > 0);
	// 			cudaUpdateType[c_pGpuNet->nTypes[i]](c_pGpuNet->pNeurons[i], c_pGpuNet->neuronNums[i+1]-c_pGpuNet->neuronNums[i], c_pGpuNet->neuronNums[i], time, &updateSize[c_pGpuNet->nTypes[i]]);
	// 		}
	// 	}

	// 	cudaMemset(c_g_fired_n_num, 0, sizeof(int)*network->_node_num);
	// 	cudaDeliverNeurons(c_g_idx2index, c_g_cross_index2idx, c_g_global_cross_data, c_g_fired_n_num, network->_node_num, allNeuronNum, time);
	// 	checkCudaErrors(cudaMemcpy(gCrossDataNet->_fired_num + network->_node_idx * network->_node_num, c_g_fired_n_num, sizeof(int)*network->_node_num, cudaMemcpyDeviceToHost));

	// 	for (int i=0; i< network->_node_num; i++) {
	// 		int idx2i = network->_node_idx * network->_node_num + i;
	// 		assert(gCrossDataNet->_fired_num[idx2i] <= gCrossDataNet->_max_num[idx2i]);
	// 		if (gCrossDataNet->_fired_num[idx2i] > 0) {
	// 			checkCudaErrors(cudaMemcpyPeer(gCrossDataNet->_fired_arrays[idx2i], i, c_g_global_cross_data + allNeuronNum * i, network->_node_idx, gCrossDataNet->_fired_num[idx2i] * sizeof(int)));
	// 		}
	// 	}

// #ifde// f LOG_DATA
// 	// 	int currentIdx = time%(MAX_DELAY+1);
// 
// 	// 	int copySize = 0;
// 	// 	copyFromGPU<int>(&copySize, buffers->c_gFiredTableSizes + currentIdx, 1);
// 	// 	if (copySize > 0) {
// 	// 		copyFromGPU<int>(buffers->c_neuronsFired, buffers->c_gFiredTable + (allNeuronNum*currentIdx), copySize);
// 	// 	}
// 
// 	// 	if (copy_idx >= 0 && (c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]) > 0) {
// 	// 		copyFromGPU<real>(c_vm, c_g_vm, c_pGpuNet->neuronNums[copy_idx+1]-c_pGpuNet->neuronNums[copy_idx]);
// 	// 	}
// #endi// f

	// 	for (int i=0; i<sTypeNum; i++) {
	// 		assert(c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i] > 0);
	// 		cudaUpdateType[pCpuNet->sTypes[i]](c_pGpuNet->pSynapses[i], c_pGpuNet->synapseNums[i+1]-c_pGpuNet->synapseNums[i], c_pGpuNet->synapseNums[i], time, &updateSize[pCpuNet->sTypes[i]]);
	// 	}

	// 	for (int i=0; i< network->_node_num; i++) {
	// 		int i2idx = network->_node_idx + network->_node_num * i;
	// 		if (gCrossDataNet->_fired_num[i2idx] > 0) {
	// 			addCrossNeurons(gCrossDataNet->_fired_arrays[i2idx], gCrossDataNet->_fired_num[i2idx], time);
	// 		}
	// 	}
	// 	
// #ifde// f LOG_DATA
// 	// 	for (int i=0; i<copySize; i++) {
// 	// 		fprintf(log_file, "%d ", buffers->c_neuronsFired[i]);
// 	// 	}
// 	// 	fprintf(log_file, "\n");
// 
// 	// 	for (int i=0; i<c_pGpuNet->neuronNums[copy_idx+1] - c_pGpuNet->neuronNums[copy_idx]; i++) {
// 	// 		fprintf(v_file, "%.10lf \t", c_vm[i]);
// 	// 	}
// 	// 	fprintf(v_file, "\n");
// #endi// f

	// }
	// gettimeofday(&te, NULL);
	// long seconds = te.tv_sec - ts.tv_sec;
	// long hours = seconds/3600;
	// seconds = seconds%3600;
	// long minutes = seconds/60;
	// seconds = seconds%60;
	// long uSeconds = te.tv_usec - ts.tv_usec;
	// if (uSeconds < 0) {
	// 	uSeconds += 1000000;
	// 	seconds = seconds - 1;
	// }

	// printf("Simulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	// int *rate = (int*)malloc(sizeof(int)*nodeNeuronNum);
	// copyFromGPU<int>(rate, buffers->c_gFireCount, nodeNeuronNum);

	// char fire_filename[512];
	// sprintf(fire_filename, "GFire_%d.log", network->_node_idx); 
	// FILE *rate_file = fopen(fire_filename, "w+");
	// if (rate_file == NULL) {
	// 	printf("Open file Sim.log failed\n");
	// 	return NULL;
	// }

	// for (int i=0; i<nodeNeuronNum; i++) {
	// 	fprintf(rate_file, "%d \t", rate[i]);
	// }

	// free(rate);
	// fclose(rate_file);

	// fclose(log_file);
	// fclose(v_file);

	// free_buffers(buffers);
	// freeGPUNetwork(c_pGpuNet);

	// return NULL;







	return 0;
}

