/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "LIFNeuron.h"
#include "GLIFNeuron.h"

using std::vector;

LIFNeuron::LIFNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	:v_init(v_init), v_rest(v_rest), v_reset(v_reset), cm(cm), tau_m(tau_m), tau_refrac(tau_refrac), tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I), v_thresh(v_thresh), i_offset(i_offset)
{
	this->i_syn = 0;
	this->id = id;
	this->file = NULL;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron, ID id)
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
	this->id = id;
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
		fire();
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

ID LIFNeuron::getID()
{
	return id;
}

real LIFNeuron::get_vm()
{
	return vm;
}

void LIFNeuron::monitor() 
{
	if (file == NULL) {
		char filename[128];
		sprintf(filename, "Neuron_%d.log", id.id);
		file = fopen(filename, "w+");
	}
	fprintf(file, "%lf\n", this->vm); 
}

size_t LIFNeuron::getSize()
{
	return sizeof(GLIFNeurons);
}

unsigned int LIFNeuron::hardCopy(void* data, unsigned int idx)
{
	GLIFNeurons * p = (GLIFNeurons *) data;
	p->pID[idx] = id;
	p->pType[idx] = type;
	p->p_v_init[idx] = v_init;
	p->p_v_rest[idx] = v_rest;
	p->p_v_reset[idx] = v_reset;
	p->p_cm[idx] = cm;
	p->p_tau_m[idx] = tau_m;
	p->p_tau_refrac[idx] = tau_refrac;
	p->p_tau_syn_E[idx] = tau_syn_E;
	p->p_tau_syn_I[idx] = tau_syn_I;
	p->p_v_thresh[idx] = v_thresh;
	p->p_i_offset[idx] = i_offset;
	p->p_i_syn[idx] = i_syn;
	p->p_vm[idx] = vm;
	p->p__dt[idx] = _dt;
	p->p_C1[idx] = C1;
	p->p_C2[idx] = C2;
	p->p_i_tmp[idx] = i_tmp;
	p->p_refrac_step[idx] = refrac_step;
	
	return 1;
}
