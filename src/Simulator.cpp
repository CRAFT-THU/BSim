/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <stdio.h>
#include "Simulator.h"

Simulator::Simulator(Connector *network, real dt)
	: network(network), dt(dt)
{
}

Simulator::~Simulator()
{
}

int Simulator::run(real time)
{
	int sim_time = (int) (time/dt);

	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
		SynapseBase *p = *iterS;
		p->reset(dt);
	}
	for (iterN=network->pNeurons.begin(); iterN!=network->pNeurons.end(); iterN++) {
		NeuronBase * p = *iterN;
		p->reset(dt);
	}

	printf("Start runing for %d cycles...\n", sim_time);
	for (int i=0; i<sim_time; i++) {
		//printf("Cycle: %d\n", i);
		for (iterS=network->pSynapses.begin(); iterS!=network->pSynapses.end(); iterS++) {
			SynapseBase *p = *iterS;
			p->update();
			p->monitor();
		}

		for (iterN=network->pNeurons.begin(); iterN!=network->pNeurons.end(); iterN++) {
			NeuronBase * p = *iterN;
			p->update();
			if (p->is_fired()) {
				p->fire();
			}
			p->monitor();
		}
	}
	printf("Finish runing\n");

	return 0;
}
