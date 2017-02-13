/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <chrono>

#include "GPossionNeurons.h"
#include "PossionNeuron.h"

const Type PossionNeuron::type = Possion;

PossionNeuron::PossionNeuron(ID id, real rate, real refract, real startTime, real duration)
{
	this->id = id;
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

PossionNeuron::~PossionNeuron()
{
	pSynapses.clear();
}

int PossionNeuron::reset(SimInfo &info)
{
	fired = false;
	startCycle = (int) startTime/info.dt;
	startCycle += distribution(generator);
	fireCycle = startCycle;
	endCycle = (int) endTime/info.dt;
	refract_step = (int) refract/info.dt;
	return 0;
}

Type PossionNeuron::getType()
{
	return type;
}

int PossionNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle == fireCycle && fireCycle < endCycle) {
		fired = true;
		fireCycle = fireCycle + 1 + distribution(generator); 
		fire();
		info.fired.push_back(this->id);
	} 
	return 0;
}

void PossionNeuron::monitorOn()
{
	monitored = true;
}

void PossionNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "PossionNeuron_%d_%d.log", this->id.groupId, this->id.id);
			file = fopen(filename, "w+");
		}
		fprintf(file, "%d:%d:%d:%d\n", info.currCycle, fireCycle, startCycle, fired);
	}
	return;
}

bool PossionNeuron::isFired()
{
	return fired;
}

int PossionNeuron::recv(real I) {
	return 0;
}

size_t PossionNeuron::getSize() {
	return sizeof(GPossionNeurons);
}


int PossionNeuron::hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GPossionNeurons *p = (GPossionNeurons *) data;
	id2idx[id] = idx + base;
	idx2id[idx+base] = id;

	p->p_rate[idx] = rate;
	p->p_fire_cycle[idx] = fireCycle;
	p->p_end_cycle[idx] = endCycle;
	p->p_refrac_step[idx] = refract_step;

	return 0;
}

int PossionNeuron::getData(void *p)
{
	return 0;
}
