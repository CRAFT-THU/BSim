/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <stdio.h>
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
	FILE *outFile = fopen("Output.csv", "w+");
	if (logFile == NULL) {
		printf("Open file Sim.log failed\n");
		return -1;
	}
	if (outFile == NULL) {
		printf("Open file Sim.log failed\n");
		return -2;
	} else {
		fprintf(outFile, "Cycle");
		int size = network->pOutputs.size();
		for (int i=0; i<size; i++) {
			fprintf(outFile, ",%d_%d", network->pOutputs[i]->getID().groupId, network->pOutputs[i]->getID().id);
		}
		fprintf(outFile, "\n");
	}

	reset();
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	printf("Start runing for %d cycles\n", sim_cycle);
	for (int cycle=0; cycle<sim_cycle; cycle++) {
		//printf("\rCycle: %d", cycle);
		fflush(stdout);

		info.currCycle = cycle;
		info.fired.clear();

		//Update
		for (iterP=network->pPopulations.begin(); iterP!=network->pPopulations.end(); iterP++) {
			PopulationBase * p = *iterP;
			p->update(info);
		}

		for (iterN=network->pNeurons.begin(); iterN!=network->pNeurons.end(); iterN++) {
			NeuronBase * p = *iterN;
			p->update(info);
		}

		for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
			SynapseBase *p = *iterS;
			p->update(info);
		}

		//Log info
		for (iterP=network->pPopulations.begin(); iterP!=network->pPopulations.end(); iterP++) {
			PopulationBase * p = *iterP;
			p->monitor(info);
		}

		for (iterN=network->pNeurons.begin(); iterN!=network->pNeurons.end(); iterN++) {
			NeuronBase * p = *iterN;
			p->monitor(info);
		}

		for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
			SynapseBase *p = *iterS;
			p->monitor(info);
		}

		int size = info.fired.size();
		if (size > 0) {
			fprintf(logFile, "Cycle %d: ", info.currCycle);
			fprintf(logFile, "%d_%d", info.fired[0].groupId, info.fired[0].id);
			for (int i=1; i<size; i++) {
				fprintf(logFile, ", %d_%d", info.fired[i].groupId, info.fired[i].id);
			}
			fprintf(logFile, "\n");
		}
		size = network->pOutputs.size();
		if (size > 0) {
			fprintf(outFile, "%d", info.currCycle);
			for (int i=1; i<size; i++) {
				fprintf(outFile, ",%d", network->pOutputs[i]->isFired());
			}
			fprintf(outFile, "\n");
		}
	}

	fclose(logFile);
	printf("\nFinish runing\n");

	return 0;
}
