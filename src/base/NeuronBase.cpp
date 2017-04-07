/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include <stdio.h>

#include "NeuronBase.h"

NeuronBase::NeuronBase(ID id, int node) : Base(id, node)
{
	fired = false;
	monitored = false;
	fireCount = 0;
}

NeuronBase::~NeuronBase() {}

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
	return 0;
}

SynapseBase * NeuronBase::createSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *dst) 
{
	return NULL;
}

SynapseBase * NeuronBase::addSynapse(SynapseBase * synapse)
{
	return NULL;
}
