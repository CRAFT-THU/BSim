/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include "LIFNeuron.h"
#include <vector>
#include <math.h>
using std::vector;

LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	:v_init(v_init), v_rest(v_rest), v_reset(v_reset), cm(cm), tau_m(tau_m), tau_refrac(tau_refrac), tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I), v_thresh(v_thresh), i_offset(i_offset)
{
	this->i_syn = 0;
	this->id = nid.getID();
	this->file = NULL;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron)
{
	this->v_init = neuron.v_init;
	this->v_rest = neuron.v_rest;
	this->v_reset = neuron.v_reset;
	this->cm = neuron.cm;
	this->tau_m = neuron.tau_m;
	this->tau_refrac = neuron.tau_refrac;
	this->tau_syn_E = neuron.tau_syn_E;
	this->tau_syn_I = neuron.tau_syn_I;
	this->v_thresh = neuron.v_thresh;
	this->i_offset = neuron.i_offset;
	this->i_syn = 0;
	this->file = NULL;
	id = nid.getID();
}

LIFNeuron::~LIFNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

bool LIFNeuron::is_fired()
{
	return fired;
}

int LIFNeuron::init(real dt)
{
	_dt = dt;
	real rm = cm/tau_m;
	if (tau_m > 0) {
		C1 = expf(-dt/tau_m);
		C2 = rm*(1-C1);
	} else {
		C1 = 0.0f;
		C2 = rm;
	}

	if (rm > 0) {
		i_tmp = i_offset + v_rest/rm;
	}

	refrac_step = tau_refrac/dt;
	
	printf("R: %f, C: %f, T:%f\n", rm, cm, tau_m);
	printf("C1: %f, C2: %f\n", C1, C2);
	printf("i_offset: %f, vrest: %f\n", i_offset, v_rest);
	printf("T_ref: %f, dt: %f\n", tau_refrac, _dt);

	return 0;
}

int LIFNeuron::update()
{
	fired = false;

	if (refrac_step > 0) {
		--refrac_step;
	} else {
		real I = i_syn + i_tmp;
		vm = C1 * vm + C2 * I;

		if (vm >= v_thresh) {
			fired = true;
			refrac_step = (int)(tau_refrac/_dt) - 1;
			vm = v_reset;
		}

		//printf("C1: %f, C2: %f\n", C1, C2);
		//printf("i_offset: %f, vrest: %f\n", i_offset, v_rest);
		//printf("T_ref: %f, dt: %f\n", tau_refrac, _dt);
		//printf("vm = %f, v_thresh:%f, v_reset:%f\n", vm, v_thresh, v_reset);
	}

	i_syn = 0;

	if (fired) {
	//TODO fire
		return 1;
	} else {
		return -1;
	}
}

int LIFNeuron::recv(real I)
{
	i_syn += I;
	return 0;
}

int LIFNeuron::reset(real dt)
{
	vm = v_init;
	refrac_step = -1;
	i_syn = 0;
	return init(dt);
}

real LIFNeuron::get_vm()
{
	return vm;
}

void LIFNeuron::monitor() 
{
	if (file == NULL) {
		char filename[128];
		sprintf(filename, "Neuron_%d.log", id);
		file = fopen(filename, "w+");
	}
	fprintf(file, "%lf", this->vm); 
}
