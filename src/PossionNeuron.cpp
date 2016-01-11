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
	fired = false;
	m_fireCycle= 0;
	m_currCycle = 0;
	m_startCycle = 0;
}

PossionNeuron::~PossionNeuron()
{
	pSynapses.clear();
}

int PossionNeuron::reset(real dt)
{
	fired = false;
	_dt = dt;
	m_currCycle = 0;
	m_startCycle = m_startTime/dt;
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

int PossionNeuron::update()
{
	fired = false;
	m_currCycle++;
	if (m_currCycle == m_fireCycle && m_currCycle > m_startCycle) {
		fired = true;
		m_fireCycle = possion(m_fireCycle);
		fire();
	}
	return 0;
}

void PossionNeuron::monitor()
{
	return;
}

bool PossionNeuron::is_fired()
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

