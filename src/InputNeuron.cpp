/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "InputNeuron.h"

const Type InputNeuron::type = Input;

InputNeuron::InputNeuron(ID id)
	:m_id(id)
{
	file = NULL;
	fired = false;
	monitored = false;
	tau_syn_E = 1e-3;
	tau_syn_I = 1e-3;

}

InputNeuron::InputNeuron(const InputNeuron &templ, ID id)
	:m_id(id)
{
	file = NULL;
	fired = false;
	monitored = templ.monitored;
	tau_syn_E = templ.tau_syn_E;
	tau_syn_I = templ.tau_syn_I;
}

InputNeuron::~InputNeuron()
{
	fireTime.clear();
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int InputNeuron::reset(SimInfo &info)
{
	fired = false;
	_dt = info.dt;
	
	return 0;
}

int InputNeuron::fire()
{
	//vector<SynapseBase*>::iterator iter;
	//for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
	//	(*iter)->recv();
	//}

	return 0;
}

ID InputNeuron::getID()
{
	return m_id;
}

Type InputNeuron::getType()
{
	return type;
}

SynapseBase * InputNeuron::addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest)
{
	//ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, 1e-3);
	//tmp->setDst(pDest);

	//SynapseBase *ret = (SynapseBase *)tmp;
	//pSynapses.push_back(ret);
	//return ret;
	return NULL;
}

int InputNeuron::update(SimInfo &info)
{
	fired = false;
	if (!fireTime.empty()) {
		while (fireTime.front() <= info.currCycle) {
			fired = true;
			fire();
			info.fired.push_back(this->m_id);
			fireTime.pop_front();
			if (fireTime.empty()) {
				break;
			}
		}
	}
	return 0;
}

void InputNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			int size = fireTime.size();
			if (size > 0) {
				char filename[128];
				sprintf(filename, "InputNeuron_%d_%d.log", this->m_id.groupId, this->m_id.id);
				file = fopen(filename, "w+");
				if (file == NULL) {
					printf("Open File: %s failed\n", filename);
					return;
				}
				fprintf(file, "Size: %d\n", size);
				for (int i=0; i<size; i++) {
					fprintf(file, "%d\n", fireTime[i]);
				}
			}
		}
	}
	return;
}

int InputNeuron::addFireTime(int cycle)
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

int InputNeuron::hardCopy(void *data, int idx)
{
	return 0;
}

int InputNeuron::getData(void *data)
{
	return 0;
}
