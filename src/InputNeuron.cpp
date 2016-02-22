/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "InputNeuron.h"
#include "ExpSynapse.h"

InputNeuron::InputNeuron(ID id)
	:m_id(id)
{
	file = NULL;
	fired = false;
	monitored = false;
}

InputNeuron::~InputNeuron()
{
	pSynapses.clear();
	fireTime.clear();
}

int InputNeuron::reset(SimInfo &info)
{
	fired = false;
	_dt = info.dt;
	
	return 0;
}

int InputNeuron::fire()
{
	vector<SynapseBase*>::iterator iter;
	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}

bool InputNeuron::isFired()
{
	return fired;
}

ID InputNeuron::getID()
{
	return m_id;
}

SynapseBase * InputNeuron::addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest)
{
	ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, 1e-3);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

int InputNeuron::update(SimInfo &info)
{
	fired = false;
	if (!fireTime.empty()) {
		if (fireTime.front() == info.currCycle) {
			fired = true;
			fire();
			fireTime.pop_front();
		}
	}
	return 0;
}

void InputNeuron::monitorOn()
{
	monitored = true;
}

void InputNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (fired) {
			if (file == NULL) {
				char filename[128];
				sprintf(filename, "InputNeuron_%d_%d.log", this->m_id.groupId, this->m_id.id);
				file = fopen(filename, "w+");
				if (file == NULL) {
					printf("Open File: %s failed\n", filename);
					return;
				}
			}
			fprintf(file, "%d\n", info.currCycle); 
		}
	}
	return;
}

int InputNeuron::addFireTime(unsigned int cycle)
{
	fireTime.push_back(cycle);
	return 0;
}

int InputNeuron::recv(real I) {
	return 0;
}

size_t InputNeuron::getSize() {
	return 0;
}

unsigned int InputNeuron::hardCopy(void *data, unsigned int idx)
{
	return 0;
}

int InputNeuron::getData(void *data)
{
	return 0;
}
