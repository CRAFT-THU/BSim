/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "PossionNeuron.h"
#include "ExpSynapse.h"

double uRandom()
{
	long f = rand();
	while (f==0 || f==RAND_MAX)
		f = rand();

	return (double)f/RAND_MAX;
}

PossionNeuron::PossionNeuron(ID id, double rate, double refract, double startTime)
	:m_rate(rate), m_refract(refract), m_startTime(startTime), m_id(id)
{
	file = NULL;
	fired = false;
	monitored = false;
	m_fireCycle= 0;
	m_startCycle = 0;
}

PossionNeuron::~PossionNeuron()
{
	pSynapses.clear();
}

int PossionNeuron::reset(SimInfo &info)
{
	fired = false;
	_dt = info.dt;
	m_startCycle = m_startTime/info.dt;
	m_fireCycle = m_startCycle;
	return 0;
}

int PossionNeuron::fire()
{
	vector<SynapseBase*>::iterator iter;
	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}

SynapseBase * PossionNeuron::addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest)
{
	ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, 1e-3);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

ID PossionNeuron::getID()
{
	return m_id;
}

int PossionNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle == m_fireCycle && info.currCycle > m_startCycle) {
		fired = true;
		m_fireCycle = possion(m_fireCycle);
		fire();
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
			sprintf(filename, "PossionNeuron_%d_%d.log", this->m_id.groupId, this->m_id.id);
			file = fopen(filename, "w+");
		}
		fprintf(file, "%d\n", m_fireCycle); 
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
	return 0;
}


unsigned int PossionNeuron::hardCopy(void *data, unsigned int idx)
{
	return 0;
}

unsigned int PossionNeuron::possion(unsigned int input)
{
	bool finished = false;
	unsigned int ret = 0;
	while (!finished) {
		double tmpVal = uRandom();
		unsigned int advance = -log(tmpVal)/m_rate;
		if (advance >= (unsigned int)(m_refract/_dt)) {
			finished = true;
			ret = input + advance;
		}
	}
	return ret;
}

