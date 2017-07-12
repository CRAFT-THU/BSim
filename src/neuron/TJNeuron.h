/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef TJNEURON_H
#define TJNEURON_H

#include <stdio.h>

#include "../base/NeuronBase.h"

class TJNeuron : public NeuronBase {
public:
	TJNeuron(real v_init, real v_reset, real cm, real tau_refrac, real v_thresh, real i_offset);
	TJNeuron(const TJNeuron &neuron);
	~TJNeuron();

	virtual Type getType();

	virtual real get_vm();
	virtual int init(real dt);

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void * data, int idx, int base);

	inline void setI(real i) { this->i_offset = i; }

	const static Type type;
protected:
	real v_init;
	real v_rest;
	real v_reset;
	real cm;
	real tau_refrac;
	real v_thresh;
	real i_offset;
	real i_syn;
	real vm;
	real i_tmp;
	int refrac_time;
	int refrac_step;
	FILE* file;
};

#endif /* TJNEURON_H */

