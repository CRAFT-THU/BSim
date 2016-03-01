/* This program is writen by qp09.
 * usually just for fun.
 * Sun October 25 2015
 */

#include "LowpassSynapse.h"
#include "Nengo_lowpass.h"

Nengo_lowpass::Nengo_lowpass(ID id, real v_init, real v_min, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset) : NengoNeuron(id, v_init, v_min, v_reset, cm, tau_m, tau_refrac, tau_syn_E, tau_syn_I, v_thresh, i_offset)
{
	type = Nengo;
}

Nengo_lowpass::Nengo_lowpass(const Nengo_lowpass &neuron, ID id) : NengoNeuron(id, neuron.v_init, neuron.v_min, neuron.v_reset, neuron.cm, neuron.tau_m, neuron.tau_refrac, neuron.tau_syn_E, neuron.tau_syn_I, neuron.v_thresh, neuron.i_offset)
{
}

Nengo_lowpass::~Nengo_lowpass()
{
	pSynapses.clear();
}

SynapseBase* Nengo_lowpass::addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest)
{
	real tau = 0.0f;
	if (type == Excitatory) {
		tau = tau_syn_E;
	} else {
		tau = tau_syn_I;
	}

	//ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, tau);
	LowpassSynapse *tmp = new LowpassSynapse(sidPool.getID(), weight, delay, tau);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

int Nengo_lowpass::fire()
{
	vector<SynapseBase*>::iterator iter;
	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}
