/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include "SingleThreadSimulator.h"

SingleThreadSimulator::SingleThreadSimulator(Network *network, real dt)
	: SimulatorBase(network, dt)
{
}

SingleThreadSimulator::~SingleThreadSimulator()
{
}

int SingleThreadSimulator::run(real time)
{
	int sim_cycle =  round((time)/dt);

	FILE *logFile = fopen("Sim.log", "w+");
	FILE *dataFile = fopen("Sim.data", "w+");
	//FILE *outFile = fopen("Output.csv", "w+");
	if (logFile == NULL) {
		printf("Open file Sim.log failed\n");
		return -1;
	}
	if (dataFile == NULL) {
		printf("Open file Sim.data failed\n");
		return -1;
	}

	//if (outFile == NULL) {
	//	printf("Open file Sim.log failed\n");
	//	return -2;
	//} else {
	//	fprintf(outFile, "Cycle");
	//	int size = network->pOutputs.size();
	//	for (int i=0; i<size; i++) {
	//		fprintf(outFile, ",%s", network->pOutputs[i]->getID().getInfo().c_str());
	//	}
	//	fprintf(outFile, "\n");
	//}

	reset();
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int cycle=0; cycle<sim_cycle; cycle++) {
		//printf("\rCycle: %d", cycle);
		//fflush(stdout);

		info.currCycle = cycle;
		info.fired.clear();
		info.input.clear();

		//Update
		network->update(info);

		//Log info
		network->monitor(info);


		int isize = info.input.size();
		//fprintf(dataFile, "Cycle %d: ", info.currCycle);
		for (int i=0; i<isize; i++) {
			fprintf(dataFile, "%lf ", info.input[i]);
		}

		fprintf(dataFile, "\n");

		int size = info.fired.size();
		fprintf(logFile, "Cycle %d: ", info.currCycle);
		for (int i=0; i<size; i++) {
			fprintf(logFile, "%s ", info.fired[i].getInfo().c_str());
		}
		fprintf(logFile, "\n");

		//size = network->pOutputs.size();
		//fprintf(outFile, "%d", info.currCycle);
		//if (size > 0) {
		//	fprintf(outFile, ",%d", network->pOutputs[0]->isFired());
		//	for (int i=1; i<size; i++) {
		//		fprintf(outFile, ",%d", network->pOutputs[i]->isFired());
		//	}
		//}
		//fprintf(outFile, "\n");
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

	fclose(logFile);
	printf("\nSimulation finished in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	return 0;
}
