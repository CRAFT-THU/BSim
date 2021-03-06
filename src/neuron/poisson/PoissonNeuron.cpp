/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <chrono>

#include "GPoissonNeurons.h"
#include "PoissonNeuron.h"

const Type PoissonNeuron::type = Poisson;

PoissonNeuron::PoissonNeuron(real rate, real refract, real startTime, real duration) : NeuronBase()
{
	this->rate = rate;
	this->refract = refract;
	this->startTime = startTime;
	this->endTime = startTime + duration;
	file = NULL;
	fired = false;
	monitored = false;
	fireCycle= 0;
	startCycle = 0;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	generator.seed(seed);
	poisson_distribution<int> distribution_t(rate);
	distribution.param(distribution_t.param());
}

PoissonNeuron::~PoissonNeuron()
{
	pSynapses.clear();
}

int PoissonNeuron::reset(SimInfo &info)
{
	fired = false;
	startCycle = static_cast<int>(startTime/info.dt);
	startCycle += distribution(generator);
	fireCycle = startCycle;
	endCycle = static_cast<int>(endTime/info.dt);
	refract_step = static_cast<int>(refract/info.dt);
	return 0;
}

Type PoissonNeuron::getType()
{
	return type;
}

int PoissonNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle == fireCycle && fireCycle < endCycle) {
		fired = true;
		fireCycle = fireCycle + 1 + distribution(generator); 
		fire();
		fire_count++;
	} 
	info.fired.push_back(fired);
	return 0;
}

void PoissonNeuron::monitorOn()
{
	monitored = true;
}

void PoissonNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "PoissonNeuron_%d.log", getID());
			file = fopen(filename, "w+");
		}
		fprintf(file, "%d:%d:%d:%d\n", info.currCycle, fireCycle, startCycle, fired);
	}
	return;
}

int PoissonNeuron::recv(real I) {
	return 0;
}

size_t PoissonNeuron::getSize() {
	return sizeof(GPoissonNeurons);
}


int PoissonNeuron::hardCopy(void *data, int idx, int base)
{
	GPoissonNeurons *p = (GPoissonNeurons *) data;
	setID(idx+base);

	p->p_rate[idx] = rate;
	p->p_fire_cycle[idx] = fireCycle;
	p->p_end_cycle[idx] = endCycle;
	p->p_refrac_step[idx] = refract_step;

	return 0;
}

int PoissonNeuron::getData(void *p)
{
	return 0;
}
