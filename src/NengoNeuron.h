/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef NENGONEURON_H
#define NENGONEURON_H

#include <stdio.h>

#include "NeuronBase.h"

class NengoNeuron : public NeuronBase {
public:
	NengoNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	NengoNeuron(const NengoNeuron &neuron, ID id);
	~NengoNeuron();

	ID getID();

	virtual real get_vm();
	virtual int init(real dt);

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual bool isFired();

	virtual size_t getSize();
	virtual int recv(real I);
	virtual int getData(void *data);
	virtual unsigned int hardCopy(void * data, unsigned int idx);
protected:
	ID id;
	NeuronType type;
	real v_init;
	real v_min;
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
	real _dt;
	real C1;
	real C2;
	real i_tmp;
	int refrac_step;
	bool fired;
	bool monitored;
	FILE* file;
};

#endif /* NENGONEURON_H */

