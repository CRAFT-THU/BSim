#ifndef LIFCURREXPNEURON_H
#define LIFCURREXPNEURON_H

#include <stdio.h>
#include "../../base/NeuronBase.h"


class LIFCurrExpNeuron : public NeuronBase {
public:
	LIFCurrExpNeuron(ID id, real i_offset, real i_injection, real cm, real tau_m, real tau_e, real tau_i, real vm, real v_thresh, real v_reset, real v_reset, real i_E, real i_I, int refrac_time, int refrac_step);
	LIFCurrExpNeuron(const LIFCurrExpNeuron &neuron, ID id);
	~LIFCurrExpNeuron();

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
	real _i_offset;
	real _i_injection;
	real _cm;
	real _tau_m;
	real _tau_e;
	real _tau_i;
	real _vm;
	real _v_thresh;
	real _v_reset;
	real _v_reset;
	real _i_E;
	real _i_I;
	int _refrac_time;
	int _refrac_step;
};

#endif /* LIFCURREXPNEURON_H */
