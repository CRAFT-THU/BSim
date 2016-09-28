/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <time.h>
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

const Type PossionNeuron::type = Possion;

PossionNeuron::PossionNeuron(ID id, double rate, double refract, double startTime)
	:m_rate(rate), m_refract(refract), m_startTime(startTime)
{
	this->id = id;
	file = NULL;
	fired = false;
	monitored = false;
	m_fireCycle= possion(0);
	m_startCycle = 0;
	srand(time(NULL));
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
	m_fireCycle = possion(m_startCycle);
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

SynapseBase * PossionNeuron::addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest)
{
	ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, 1e-3);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

Type PossionNeuron::getType()
{
	return type;
}

int PossionNeuron::update(SimInfo &info)
{
	fired = false;
	info.input.push_back(m_fireCycle);
	if (info.currCycle == m_fireCycle && info.currCycle >= m_startCycle) {
		fired = true;
		m_fireCycle = possion(m_fireCycle);
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
		fprintf(file, "%d:%d:%d:%d\n", info.currCycle, m_fireCycle, m_startCycle, fired);
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


int PossionNeuron::hardCopy(void *data, int idx)
{
	return 0;
}

int PossionNeuron::possion(int input)
{
	bool finished = false;
	int ret = 0;
	while (!finished) {
		double tmpVal = uRandom();
		int advance = -log(tmpVal)/m_rate;
		if (advance >= (int)(m_refract/_dt)) {
			finished = true;
			ret = input + 1 + advance;
		}
	}
	return ret;
}

int PossionNeuron::getData(void *p)
{
	return 0;
}
