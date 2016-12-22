/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "ConstantNeuron.h"

const Type ConstantNeuron::type = Constant;

ConstantNeuron::ConstantNeuron(ID id, real fire_rate)
{
	this->id = id;
	this->fire_rate = fire_rate;
	file = NULL;
	fired = false;
	monitored = false;

}

ConstantNeuron::ConstantNeuron(const ConstantNeuron &templ, ID id)
{
	this->id = id;
	file = NULL;
	fired = false;
	this->fire_rate = templ.fire_rate;
	monitored = templ.monitored;
}

ConstantNeuron::~ConstantNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int ConstantNeuron::reset(SimInfo &info)
{
	fired = false;
	this->_dt = info.dt;
	this->fire_count = (int)(1.0/(this->_dt)*(this->fire_rate));
	
	return 0;
}

int ConstantNeuron::fire()
{
	//vector<SynapseBase*>::iterator iter;
	//for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
	//	(*iter)->recv();
	//}

	return 0;
}

Type ConstantNeuron::getType()
{
	return type;
}

SynapseBase * ConstantNeuron::addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest)
{
	//ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, 1e-3);
	//tmp->setDst(pDest);

	//SynapseBase *ret = (SynapseBase *)tmp;
	//pSynapses.push_back(ret);
	//return ret;
	return NULL;
}

int ConstantNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle < fire_count) {
		fired = true;
		fire();
		info.fired.push_back(this->id);
	}
	return 0;
}

void ConstantNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "ConstantNeuron_%d_%d.log", this->id.groupId, this->id.id);
			file = fopen(filename, "w+");
			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}
		} else {
			if (fired) {
				fprintf(file, "%d\n", info.currCycle);
			}
		}
	}
	return;
}

int ConstantNeuron::recv(real I) {
	return 0;
}

size_t ConstantNeuron::getSize() {
	return 0;
}

int ConstantNeuron::hardCopy(void *data, int idx)
{
	return 0;
}

int ConstantNeuron::getData(void *data)
{
	return 0;
}
