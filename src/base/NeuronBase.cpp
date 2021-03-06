/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include <stdio.h>

#include "NeuronBase.h"
#include "SynapseBase.h"

NeuronBase::NeuronBase(int node) : Base(node)
{
	fired = false;
	monitored = false;
	fire_count = 0;
	input_current = 0.0;
	//_start_E = -1;
	//_start_I = -1;
	//_end = -1;
}

NeuronBase::~NeuronBase() 
{
	pSynapses.clear();
}

bool NeuronBase::isFired() 
{
	return fired;
}

int NeuronBase::getFireCount() 
{
	return fire_count;
}

real NeuronBase::getInputCurrent()
{
	return input_current;
}

void NeuronBase::monitorOn() 
{
	monitored = true;
}

int NeuronBase::fire() 
{
	for (auto iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}

//void NeuronBase::setStartExec(int idx)
//{
//	this->_start_E = idx;
//}
//
//void NeuronBase::setStartInhi(int idx)
//{
//	this->_start_I = idx;
//}
//
//void NeuronBase::setEnd(int idx)
//{
//	this->_end = idx;
//}

SynapseBase * NeuronBase::createSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *dst) 
{
	return NULL;
}

SynapseBase * NeuronBase::addSynapse(SynapseBase * synapse)
{
	pSynapses.push_back(synapse);
	return synapse;
}

const vector<SynapseBase*> & NeuronBase::getSynapses() const 
{
	return pSynapses;
}
