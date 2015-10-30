/* This header file is writen by qp09
 * usually just for fun
 * Sun October 25 2015
 */
#ifndef IF_CURR_EXP_H
#define IF_CURR_EXP_H

#include <vector>
#include "LIFNeuron.h"
#include "ExpSyn.h"

using std::vector;


class IF_curr_exp : public LIFNeuron {
public:
	IF_curr_exp(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	IF_curr_exp(const IF_curr_exp &neuron);
	~IF_curr_exp();

	virtual SynapseBase* addSyn(real weight, real delay, SynType type, NeuronBase *pDest);
	virtual int fire();
	vector<ExpSyn> synapses;
};


#endif /* IF_CURR_EXP_H */

