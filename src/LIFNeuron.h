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
	LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset);
	LIFNeuron(const LIFNeuron &neuron);
	~LIFNeuron();

	int init(real dt);
	virtual int reset(real dt);
	virtual int recv(real I);
	virtual int update();
	virtual bool is_fired();
	virtual void monitor();
	virtual real get_vm();
	virtual size_t getSize();
	virtual size_t hardCopy(unsigned char* data);
protected:
	int id;
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

