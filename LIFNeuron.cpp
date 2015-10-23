/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include "LIFNeuron.h"
#include <vector>
using std::vector;

LIFNeuron::LIFNeuron(float v_init, float v_rest, float v_reset, float cm, float tau_m, float tau_refrac, float tau_syn_E, float tau_syn_I, float v_thresh, float i_offset)
	:v_init(v_init), v_rest(v_rest), v_reset(v_reset), cm(cm), tau_m(tau_m), tau_refrac(tau_refrac), tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I), v_thresh(thresh), i_offset(i_offset)
{
	this.i_syn = 0;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron)
{
	this.v_init = neuron.v_init;
	this.v_rest = neuron.v_rest;
	this.v_reset = neuron.v_reset;
	this.cm = neuron.cm;
	this.tau_m = neuron.tau_m;
	this.tau_refrac = neuron.tau_refrac;
	this.tau_syn_E = neuron.tau_syn_E;
	this.tau_syn_I = neuron.tau_syn_I;
	this.v_thresh = neuron.v_thresh;
	this.i_offset = neuron.i_offset;
	this.i_syn = 0;
}

LIFNeuron::~LIFNeuron()
{
}

int LIFNeuron::init(float dt)
{
	_dt = dt;
	float rm = cm/tau_m;
	if (tau_m > 0) {
		C1 = exp(-dt/tau_m);
		C2 = rm*(1-C1);
	} else {
		C1 = 0.0f;
		C2 = Rm;
	}

	if (rm > 0) {
		i_tmp = i_offset + v_rest/rm;
	}

	return 0;
}

int LIFNeuron::update()
{
	bool fired = false;

	if (refrac_step > 0) {
		--refrac_step;
	} else {
		float I = i_syn + i_tmp;
		vm = C1 * vm + C2 * I;

		if (vm >= v_thresh) {
			fired = true;
			refrac_step = (int)(tau_refrac/_dt) - 1;
			vm = v_reset;
		}
	}

	i_syn = 0;

	if (fired) {
	//TODO fire
		return 1;
	} else {
		return -1;
	}
}

int LIFNeuron::reset(float dt)
{
	vm = v_init;
	refrac_step = -1;
	i_syn = 0;
	return init(dt);
}
