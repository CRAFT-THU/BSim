/* This header file is writen by qp09
 * usually just for fun
 * Sun October 25 2015
 */
#ifndef IF_CURR_EXP_H
#define IF_CURR_EXP_H

#include <vector>

#include "LIFNeuron.h"

using std::vector;


class IF_curr_exp : public LIFNeuron {
public:
	IF_curr_exp(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	IF_curr_exp(const IF_curr_exp &neuron, ID id);
	~IF_curr_exp();

	SynapseBase* addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest);
	virtual int fire();

private:
	vector<SynapseBase*> pSynapses;
};


#endif /* IF_CURR_EXP_H */

