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
	fireCount = 0;
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
	return fireCount;
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
