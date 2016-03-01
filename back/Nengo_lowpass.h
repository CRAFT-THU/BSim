/* This header file is writen by qp09
 * usually just for fun
 * Sun October 25 2015
 */
#ifndef NENGO_LOWPASS_H
#define NENGO_LOWPASS_H

#include <vector>

#include "NengoNeuron.h"

using std::vector;


class Nengo_lowpass : public NengoNeuron {
public:
	Nengo_lowpass(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	Nengo_lowpass(const Nengo_lowpass &neuron, ID id);
	~Nengo_lowpass();

	SynapseBase* addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);
	virtual int fire();

private:
	vector<SynapseBase*> pSynapses;
};


#endif /* NENGO_LOWPASS_H */

