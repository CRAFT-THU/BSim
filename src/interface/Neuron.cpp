
#include "Neuron.h"

Neuron::~Neuron() 
{
	pSynapses.clear();
}

Synapse * Neuron::addSynapse(Synapse * synapse)
{
	pSynapses.push_back(synapse);
	return synapse;
}

const vector<Synapse*> & Neuron::getSynapses() const 
{
	return pSynapses;
}

