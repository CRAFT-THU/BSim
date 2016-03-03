/* This program is writen by qp09.
 * usually just for fun.
 * Sun October 25 2015
 */

#include <math.h>

#include "ExpSynapse.h"
#include "IF_curr_exp.h"

IF_curr_exp::IF_curr_exp(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset) : LIFNeuron(id, v_init, v_rest, v_reset, cm, tau_m, tau_refrac, tau_syn_E, tau_syn_I, v_thresh, i_offset)
{
	type = IF_Curr;
}

IF_curr_exp::IF_curr_exp(const IF_curr_exp &neuron, ID id) : LIFNeuron(id, neuron.v_init, neuron.v_rest, neuron.v_reset, neuron.cm, neuron.tau_m, neuron.tau_refrac, neuron.tau_syn_E, neuron.tau_syn_I, neuron.v_thresh, neuron.i_offset)
{
}

IF_curr_exp::~IF_curr_exp()
{
	pSynapses.clear();
}

SynapseBase* IF_curr_exp::addSynapse(real weight, real delay, SpikeType type, real tau_in,  NeuronBase *pDest)
{
	real tau = 0.0f;
	if (fabs(tau_in) > ZERO) {
		tau = tau_in;
	} else if (type == Excitatory) {
		tau = tau_syn_E;
	} else {
		tau = tau_syn_I;
	}

	ExpSynapse *tmp = new ExpSynapse(sidPool.getID(), weight, delay, tau);
	tmp->setDst(pDest);

	SynapseBase *ret = (SynapseBase *)tmp;
	pSynapses.push_back(ret);
	return ret;
}

int IF_curr_exp::fire()
{
	vector<SynapseBase*>::iterator iter;
	for (iter=pSynapses.begin(); iter!=pSynapses.end(); iter++) {
		(*iter)->recv();
	}

	return 0;
}
