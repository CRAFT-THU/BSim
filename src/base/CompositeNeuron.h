/* This header file is writen by qp09
 * usually just for fun
 * Wed February 24 2016
 */
#ifndef COMPOSITENEURON_H
#define COMPOSITENEURON_H

#include <math.h>
#include <vector>
#include <queue>

#include "../utils/TagPool.h"
#include "../base/SynapseBase.h"

using std::vector;

template<class Neuron, class Synapse>
class CompositeNeuron : public Neuron {
public:
	CompositeNeuron(real tau_syn_E, real tau_syn_I);
	CompositeNeuron(const Neuron &templ, real tau_syn_E = 1e-3, real tau_syn_I = 1e-3);
	CompositeNeuron(const CompositeNeuron<Neuron, Synapse> &templ);
	~CompositeNeuron();

	//virtual SynapseBase* addSynapse(SynapseBase *synapse) override;
	virtual SynapseBase* createSynapse(real weight, real delay, SpikeType type, real tau_in, NeuronBase *pDest) override;
	//virtual int fire() override;
	//virtual int setNode(int node);
private:
	real _tau_syn_E;
	real _tau_syn_I;
	//vector<SynapseBase*> pPreSynapses;
};

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::CompositeNeuron(real tau_syn_E, real tau_syn_I) {
	this->_tau_syn_E = tau_syn_E;
	this->_tau_syn_I = tau_syn_I;
	exit(-1);
}

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::CompositeNeuron(const Neuron &templ, real tau_syn_E, real tau_syn_I) : Neuron(templ) {
	this->_tau_syn_E = tau_syn_E;
	this->_tau_syn_I = tau_syn_I;
}

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::CompositeNeuron(const CompositeNeuron<Neuron, Synapse> &templ) : Neuron(templ) {
	this->_tau_syn_E = templ._tau_syn_E;
	this->_tau_syn_I = templ._tau_syn_I;
}

template<class Neuron, class Synapse>
CompositeNeuron<Neuron, Synapse>::~CompositeNeuron()
{
	//pSynapses.clear();
	//pPreSynapses.clear();
}

template<class Neuron, class Synapse>
SynapseBase *CompositeNeuron<Neuron, Synapse>::createSynapse(real weight, real delay, SpikeType type, real tau_in, NeuronBase *pDest)
{
	real tau = 0.0f;
	if (fabs(tau_in) > ZERO) {
		tau = tau_in;
	} else if (type == Excitatory) {
		tau = this->_tau_syn_E;
	} else {
		tau = this->_tau_syn_I;
	}

	Synapse *tmp = new Synapse(weight, delay, tau);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	//pPreSynapses.push_back(ret);
	return ret;
}

//template<class Neuron, class Synapse>
//SynapseBase* CompositeNeuron<Neuron, Synapse>::addSynapse(SynapseBase * synapse)
//{
//	pSynapses.push_back(synapse);
//	return synapse;
//}

//template<class Neuron, class Synapse>
//int CompositeNeuron<Neuron, Synapse>::fire()
//{
//	vector<SynapseBase*>::iterator iter;
//	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
//		(*iter)->recv();
//	}
//
//	return 0;
//}

//template<class Neuron, class Synapse>
//int CompositeNeuron<Neuron, Synapse>::setNode(int node)
//{
//	vector<SynapseBase*>::iterator iter;
//	for (iter=pPreSynapses.begin(); iter!=pPreSynapses.end(); iter++) {
//		(*iter)->setNode(node);
//	}
//
//	return pPreSynapses.size();
//}

#endif /* COMPOSITENEURON_H */

