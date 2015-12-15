/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef LIFNEURON_H
#define LIFNEURON_H

#include <stdio.h>
#include "NeuronBase.h"

class LIFNeuron : public NeuronBase {
public:
	LIFNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	LIFNeuron(const LIFNeuron &neuron, ID id);
	~LIFNeuron();

	ID getID();
	virtual int init(real dt);
	virtual int update();
	virtual real get_vm();
	virtual void monitor();
	virtual bool is_fired();
	virtual size_t getSize();
	virtual int recv(real I);
	virtual int reset(real dt);
	virtual unsigned int hardCopy(void * data, unsigned int idx);
protected:
	ID id;
	NeuronType type;
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
	real _dt;
	real C1;
	real C2;
	real i_tmp;
	int refrac_step;
	bool fired;
	FILE* file;
};

#endif /* LIFNEURON_H */

