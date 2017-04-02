#ifndef LIFENEURON_H
#define LIFENEURON_H

#include <stdio.h>
#include "../base/NeuronBase.h"

class LIFENeuron : public NeuronBase {
public:
	LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	LIFENeuron(ID id, real CI, real vm, real CE, int refrac_step, int refrac_time, real i_tmp, real i_I, real i_E, real v_thresh, real v_reset, real C2, real C1);
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

protected:
	const static Type type;
	real _CI;
	real _vm;
	real _CE;
	int _refrac_step;
	int _refrac_time;
	real _i_tmp;
	real _i_I;
	real _i_E;
	real _v_thresh;
	real _v_reset;
	real _C2;
	real _C1;

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
