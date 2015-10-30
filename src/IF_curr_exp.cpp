/* This program is writen by qp09.
 * usually just for fun.
 * Sun October 25 2015
 */

#include "IF_curr_exp.h"

IF_curr_exp::IF_curr_exp(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset) : LIFNeuron(v_init, v_rest, v_reset, cm, tau_m, tau_refrac, tau_syn_E, tau_syn_I, v_thresh, i_offset)
{
	synapses.clear();
}

IF_curr_exp::IF_curr_exp(const IF_curr_exp &neuron) : LIFNeuron(neuron.v_init, neuron.v_rest, neuron.v_reset, neuron.cm, neuron.tau_m, neuron.tau_refrac, neuron.tau_syn_E, neuron.tau_syn_I, neuron.v_thresh, neuron.i_offset)
{
	//printf("Not well designed, be careful!");
	synapses.clear();
	for (vector<ExpSyn>::const_iterator iter=neuron.synapses.begin(); iter !=neuron.synapses.end(); iter++) {
		synapses.push_back(*iter);
	}
}

IF_curr_exp::~IF_curr_exp()
{
	synapses.clear();
}

SynapseBase* IF_curr_exp::addSyn(real weight, real delay, SynType type, NeuronBase *pDest)
{
	real tau = 0.0f;
	if (type == Excitatory) {
		tau = tau_syn_E;
	} else {
		tau = tau_syn_I;
	}

	ExpSyn st(weight, delay, tau);
	st.setDst(pDest);

	synapses.push_back(st);

	SynapseBase * ret = &(*(synapses.end() -1));

	return ret;
}

int IF_curr_exp::fire()
{
	vector<ExpSyn>::iterator iter;
	for (iter=synapses.begin(); iter!=synapses.end(); iter++) {
		(*iter).recv();
	}

	return 0;
}
