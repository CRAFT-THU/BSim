/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "../utils/utils.h"
#include "../utils/FileOp.h"
#include "../utils/TypeFunc.h"
#include "../neuron/lif/LIFData.h"

#include "SingleThreadSimulator.h"

int cFiredTableCap = 0;

SingleThreadSimulator::SingleThreadSimulator(Network *network, real dt)
	: Simulator(network, dt)
{
}

SingleThreadSimulator::~SingleThreadSimulator()
{
}

int SingleThreadSimulator::run(real time, FireInfo &log)
{
	int sim_cycle =  round((time)/_dt);

	reset();

	SimInfo info(_dt);

	GNetwork *pNetCPU = _network->buildNetwork(info);

	FILE *v_file = openFile("v.cpu.data", "w+");
	// FILE *input_e_file = openFile("input_e.data", "w+");
	// FILE *input_i_file = openFile("input_i.data", "w+");
	// FILE *ie_file = openFile("ie.data", "w+");
	// FILE *ii_file = openFile("ii.data", "w+");
	FILE *fire_file = openFile("fire.cpu.log", "w+");
	FILE *log_file = openFile("sim.cpu.log", "w+");

	int nTypeNum = pNetCPU->nTypeNum;
	int sTypeNum = pNetCPU->sTypeNum;
	int totalNeuronNum = pNetCPU->pNeuronNums[nTypeNum];
	int totalSynapseNum = pNetCPU->pSynapseNums[sTypeNum];
	printf("NeuronTypeNum: %d, SynapseTypeNum: %d\n", nTypeNum, sTypeNum);
	printf("NeuronNum: %d, SynapseNum: %d\n", totalNeuronNum, totalSynapseNum);

	int maxDelay = pNetCPU->pConnection->maxDelay;
	int deltaDelay = pNetCPU->pConnection->maxDelay - pNetCPU->pConnection->minDelay + 1;
	printf("maxDelay: %d minDelay: %d\n", pNetCPU->pConnection->maxDelay, pNetCPU->pConnection->minDelay);

	cFiredTableCap = totalNeuronNum;


	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);

	real *c_gNeuronInput = (real*)malloc(sizeof(real)*totalNeuronNum);
	memset(c_gNeuronInput, 0, sizeof(real)*totalNeuronNum);
	real *c_gNeuronInput_I = (real*)malloc(sizeof(real)*totalNeuronNum); 
	memset(c_gNeuronInput_I, 0, sizeof(real)*totalNeuronNum);
	int *c_gFiredTable = (int*)malloc(sizeof(int)*totalNeuronNum*(maxDelay+1));
	memset(c_gFiredTable, 0, sizeof(int)*totalNeuronNum*(maxDelay+1));
   	int *c_gFiredTableSizes = (int*)malloc(sizeof(int)*(maxDelay+1));
   	memset(c_gFiredTableSizes, 0, sizeof(int)*(maxDelay+1));

	for (int time=0; time<sim_cycle; time++) {
		//printf("\rCycle: %d", cycle);
		//fflush(stdout);

		info.currCycle = time;
		info.fired.clear();
		info.input.clear();

		int currentIdx = time % (maxDelay + 1);
		c_gFiredTableSizes[currentIdx] = 0;

		for (int i=0; i<nTypeNum; i++) {
			updateType[pNetCPU->pNTypes[i]](pNetCPU->pConnection, pNetCPU->ppNeurons[i], c_gNeuronInput, c_gNeuronInput_I, c_gFiredTable, c_gFiredTableSizes, pNetCPU->pNeuronNums[i+1]-pNetCPU->pNeuronNums[i], pNetCPU->pNeuronNums[i], time);
		}

		for (int i=0; i<sTypeNum; i++) {
			updateType[pNetCPU->pSTypes[i]](pNetCPU->pConnection, pNetCPU->ppSynapses[i], c_gNeuronInput, c_gNeuronInput_I, c_gFiredTable, c_gFiredTableSizes, pNetCPU->pSynapseNums[i+1]-pNetCPU->pSynapseNums[i], pNetCPU->pSynapseNums[i], time);
		}

#ifdef LOG_DATA
		int copy_idx = getIndex(pNetCPU->pNTypes, nTypeNum, LIF);
		LIFData *c_lif = (LIFData *)pNetCPU->ppNeurons[copy_idx];
		real *c_vm = c_lif->pV_m;

		int copySize = c_gFiredTableSizes[currentIdx];

		for (int i=0; i<pNetCPU->pNeuronNums[copy_idx+1] - pNetCPU->pNeuronNums[copy_idx]; i++) {
			fprintf(v_file, "%.10lf \t", c_vm[i]);
		}
		fprintf(v_file, "\n");

		for (int i=0; i<copySize; i++) {
			fprintf(log_file, "%d ", c_gFiredTable[totalNeuronNum*currentIdx+i]);
		}
		fprintf(log_file, "\n");
#endif
		
	}
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

	printf("\nSimulation finished in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);



	fclose(v_file);
	// fclose(input_e_file);
	// fclose(input_i_file);
	// fclose(ie_file);
	// fclose(ii_file);
	fclose(log_file);

	return 0;
}
