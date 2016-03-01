/* This header file is writen by qp09
 * usually just for fun
 * Wed February 24 2016
 */
#ifndef COMPOSITENEURON_H
#define COMPOSITENEURON_H

#include <vector>

#include "LIFNeuron.h"

using std::vector;

template<class Neuron, class Synapse>
class CompositeNeuron : public Neuron {
public:
	CompositeNeuron(const Neuron &templ, ID id);
	~CompositeNeuron();

	SynapseBase* addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);
	virtual int fire();

private:
	vector<SynapseBase*> pSynapses;
};

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::CompositeNeuron(const Neuron &templ, ID id) : Neuron(templ, id) {
}

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::~CompositeNeuron()
{
	pSynapses.clear();
}

template<class Neuron, class Synapse>
SynapseBase* CompositeNeuron<Neuron, Synapse>::addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest)
{
	real tau = 0.0f;
	if (type == Excitatory) {
		tau = this->tau_syn_E;
	} else {
		tau = this->tau_syn_I;
	}

	Synapse *tmp = new Synapse(sidPool.getID(), weight, delay, tau);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

template<class Neuron, class Synapse>
int CompositeNeuron<Neuron, Synapse>::fire()
{
	vector<SynapseBase*>::iterator iter;
	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}

#endif /* COMPOSITENEURON_H */

