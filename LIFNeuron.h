/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef LIFNEURON_H
#define LIFNEURON_H
#include "NeuronBase.h"

class LIFNeuron : NeuronBase {
public:
	LIFNeuron(float v_rest, float v_reset, float cm, float tau_m, float tau_refrac, float tau_syn_E, float tau_syn_I, float v_thresh, float i_offset);
	LIFNeuron(LIFNeuron neuron);
	~LIFNeuron();

	int init();
	virtual int reset();
	virtual int update();
protected:
	float v_init;
	float v_rest;
	float v_reset;
	float cm;
	float tau_m;
	float tau_refrac;
	float tau_syn_E;
	float tau_syn_I;
	float v_thresh;
	float i_offset;
	float i_syn;
	float vm;
	float _dt;
	float C1;
	float C2;
	float i_tmp;
	int refrac_step;
};

#endif /* LIFNEURON_H */

