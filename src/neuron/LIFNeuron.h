/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef LIFNEURON_H
#define LIFNEURON_H

#include <stdio.h>

#include "../base/NeuronBase.h"

class LIFNeuron : public NeuronBase {
public:
	LIFNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	LIFNeuron(const LIFNeuron &neuron, ID id);
	~LIFNeuron();

	Type getType() override;

	virtual real get_vm();
	virtual int init(real dt);

	virtual int recv(real I) override;

	virtual int reset(SimInfo &info) override;
	virtual int update(SimInfo &info) override;
	virtual void monitor(SimInfo &info) override;

	virtual size_t getSize() override;
	virtual int getData(void *data) override;
	virtual int hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) override;

	const static Type type;
protected:
	real v_init;
	real v_rest;
	real v_reset;
	real cm;
	real tau_m;
	real tau_refrac;
	real tau_syn_E;
	real tau_syn_I;
	real v_thresh;
	real i_offset;
	real i_syn;
	real vm;
	//real _dt;
	real C1;
	real C2;
	real i_tmp;
	int refrac_time;
	int refrac_step;
	FILE* file;
};

#endif /* LIFNEURON_H */

