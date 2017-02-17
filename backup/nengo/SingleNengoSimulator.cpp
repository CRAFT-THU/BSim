/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include "SingleNengoSimulator.h"

SingleNengoSimulator::SingleNengoSimulator(NengoNetwork *network, real dt)
{
	this->network = network;
	this->dt = dt;
}

SingleNengoSimulator::~SingleNengoSimulator()
{
	delete network;
}

int SingleNengoSimulator::run(real time)
{
	int sim_cycle =  round((time)/dt);

	FILE *logFile = fopen("Sim.log", "w+");
	if (logFile == NULL) {
		printf("Open file Sim.log failed\n");
		return -1;
	}

	vector<NengoLowpass*>::iterator iterS;
	vector<NengoBase*>::iterator iterP;

	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	for (iterP=network->pPopulations.begin(); iterP!=network->pPopulations.end(); iterP++) {
		NengoBase * p = *iterP;
		p->reset(info);
	}


	for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
		NengoLowpass *p = *iterS;
		p->reset(info);
	}


	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	for (int cycle=0; cycle<sim_cycle; cycle++) {
		printf("\rCycle: %d", cycle);
		fflush(stdout);

		info.currCycle = cycle;
		info.fired.clear();

		//Update
		for (iterP=network->pPopulations.begin(); iterP!=network->pPopulations.end(); iterP++) {
			NengoBase * p = *iterP;
			p->update(info);
		}


		for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
			NengoLowpass *p = *iterS;
			p->update(info);
		}

		//Log
		int size = info.fired.size();
		fprintf(logFile, "Cycle %d: ", info.currCycle);
		if (size > 0) {
			fprintf(logFile, "%d_%d", info.fired[0].groupId, info.fired[0].id);
			for (int i=1; i<size; i++) {
				fprintf(logFile, ", %d_%d", info.fired[i].groupId, info.fired[i].id);
			}
		}
		fprintf(logFile, "\n");
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
	printf("\nSimulation finesed in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);

	return 0;
}
