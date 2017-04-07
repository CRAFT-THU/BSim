#ifndef LIFENEURON_H
#define LIFENEURON_H

#include <stdio.h>
#include "../base/NeuronBase.h"

class LIFENeuron : public NeuronBase {
public:
	LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	LIFENeuron(const LIFENeuron &neuron, ID id);
	~LIFENeuron();

	virtual int recv(real I)  override;

	virtual Type getType() override;

	virtual int reset(SimInfo &info) override;
	virtual int update(SimInfo &info) override;
	virtual void monitor(SimInfo &info) override;

	virtual size_t getSize() override;
	virtual int getData(void *data) override;
	virtual int hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) override;

	const static Type type;
protected:
	real _CI;
	real _vm;
	real _CE;
	int _refrac_step;
	int _refrac_time;
	real _v_tmp;
	real _i_I;
	real _i_E;
	real _v_thresh;
	real _v_reset;
	real _Cm;

	real _v_init;
	real _v_rest;
	real _cm;
	real _tau_m;
	real _tau_refrac;
	real _tau_syn_E;
	real _tau_syn_I;
	real _i_offset;

	real _i_syn_E;
	real _i_syn_I;
};

#endif /* LIFENEURON_H */
