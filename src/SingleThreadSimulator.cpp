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
	int sim_cycle = (int) (time/dt);

	reset();
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	printf("Start runing for %d cycles\n", sim_cycle);
	for (int i=0; i<sim_cycle; i++) {
		printf("\rCycle: %d", i);
		fflush(stdout);

		for (iterP=network->pPopulations.begin(); iterP!=network->pPopulations.end(); iterP++) {
			PopulationBase * p = *iterP;
			p->update();
			p->monitor();
		}

		for (iterN=network->pNeurons.begin(); iterN!=network->pNeurons.end(); iterN++) {
			NeuronBase * p = *iterN;
			p->update();
			p->monitor();
		}

		for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
			SynapseBase *p = *iterS;
			p->update();
			p->monitor();
		}
	}
	printf("\nFinish runing\n");

	return 0;
}
