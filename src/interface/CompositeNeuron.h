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
#include "../interface/Synapse.h"

using std::vector;

template<class N, class S>
class CompositeNeuron : public N {
public:
	CompositeNeuron(real tau_syn_E, real tau_syn_I);
	CompositeNeuron(const N &templ, real tau_syn_E = 1e-3, real tau_syn_I = 1e-3);
	CompositeNeuron(const CompositeNeuron<N, S> &templ);
	~CompositeNeuron();

	//virtual Synapse* addSynapse(Synapse *synapse) override;
	virtual Synapse* createSynapse(real weight, real delay, SpikeType type, real tau_in, Neuron *pDest) override;
	//virtual int fire() override;
	//virtual int setNode(int node);
private:
	real _tau_syn_E;
	real _tau_syn_I;
	//vector<SynapseBase*> pPreSynapses;
};

template<class N, class S>
CompositeNeuron<N, S>::CompositeNeuron(real tau_syn_E, real tau_syn_I) {
	this->_tau_syn_E = tau_syn_E;
	this->_tau_syn_I = tau_syn_I;
	exit(-1);
}

template<class N, class S>
CompositeNeuron<N, S>::CompositeNeuron(const N &templ, real tau_syn_E, real tau_syn_I) : N(templ) {
	this->_tau_syn_E = tau_syn_E;
	this->_tau_syn_I = tau_syn_I;
}

template<class N, class S>
CompositeNeuron<N, S>::CompositeNeuron(const CompositeNeuron<N, S> &templ) : N(templ) {
	this->_tau_syn_E = templ._tau_syn_E;
	this->_tau_syn_I = templ._tau_syn_I;
}

template<class N, class S>
CompositeNeuron<N, S>::~CompositeNeuron()
{
	//pSynapses.clear();
	//pPreSynapses.clear();
}

template<class N, class S>
Synapse * CompositeNeuron<N, S>::createSynapse(real weight, real delay, SpikeType type, real tau_in, Neuron *pDest)
{
	real tau = 0.0;
	if (fabs(tau_in) > ZERO) {
		tau = tau_in;
	} else if (type == Excitatory) {
		tau = this->_tau_syn_E;
	} else {
		tau = this->_tau_syn_I;
	}

	Synapse *tmp = new S(weight, delay, tau);
	tmp->setDst(pDest);

	Synapse *ret = (Synapse *)tmp;
	//pPreSynapses.push_back(ret);
	return ret;
}

//template<class N, class S>
//SynapseBase* CompositeNeuron<N, S>::addSynapse(Synapse* synapse)
//{
//	pSynapses.push_back(synapse);
//	return synapse;
//}

//template<class N, class S>
//int CompositeNeuron<N, S>::fire()
//{
//	vector<Synapse *>::iterator iter;
//	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
//		(*iter)->recv();
//	}
//
//	return 0;
//}

//template<class N, class S>
//int CompositeNeuron<N, S>::setNode(int node)
//{
//	vector<SynapseBase*>::iterator iter;
//	for (iter=pPreSynapses.begin(); iter!=pPreSynapses.end(); iter++) {
//		(*iter)->setNode(node);
//	}
//
//	return pPreSynapses.size();
//}

#endif /* COMPOSITENEURON_H */

