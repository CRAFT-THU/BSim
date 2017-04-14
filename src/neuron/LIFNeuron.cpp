/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "../third_party/json/json.h"
#include "LIFNeuron.h"
#include "GLIFNeurons.h"

using std::vector;
using std::map;

const Type LIFNeuron::type = LIF;

LIFNeuron::LIFNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, /*real tau_syn_E, real tau_syn_I, */real v_thresh, real i_offset)
	: NeuronBase(id), v_init(v_init), v_rest(v_rest), v_reset(v_reset), cm(cm), tau_m(tau_m), tau_refrac(tau_refrac), /*tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I), */v_thresh(v_thresh), i_offset(i_offset)
{
	this->i_syn = 0;
	this->file = NULL;
	this->monitored = false;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron, ID id) : NeuronBase(id)
{
	this->v_init = neuron.v_init;
	this->v_rest = neuron.v_rest;
	this->v_reset = neuron.v_reset;
	this->cm = neuron.cm;
	this->tau_m = neuron.tau_m;
	this->tau_refrac = neuron.tau_refrac;
	//this->tau_syn_E = neuron.tau_syn_E;
	//this->tau_syn_I = neuron.tau_syn_I;
	this->v_thresh = neuron.v_thresh;
	this->i_offset = neuron.i_offset;
	this->i_syn = 0;
	this->file = NULL;
}

LIFNeuron::~LIFNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int LIFNeuron::init(real dt)
{
	//_dt = dt;
	real rm = 1.0;
	if (fabs(cm) > ZERO) {
		rm = tau_m/cm;
	}
	if (tau_m > 0) {
		C1 = exp(-dt/tau_m);
		C2 = rm*(1-C1);
	} else {
		C1 = 0.0f;
		C2 = rm;
	}

	if (rm > 0) {
		i_tmp = i_offset + v_rest/rm;
	} else {
		i_tmp = 0;
	}

	refrac_time = static_cast<int>(tau_refrac/dt);
	refrac_step = refrac_time;
	
	return 0;
}

int LIFNeuron::update(SimInfo &info)
{
	fired = false;

	if (refrac_step > 0) {
		--refrac_step;
	} else {
		real I = i_syn + i_tmp;
		vm = C1 * vm + C2 * I;
		
		if (vm >= v_thresh) {
			fired = true;
			//refrac_step = (int)(tau_refrac/_dt) - 1;
			refrac_step = refrac_time - 1;
			vm = v_reset;
		}

		info.input.push_back(vm);
	}

	i_syn = 0;

	if (fired) {
	//TODO fire
		fire();
		info.fired.push_back(getID());
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

int LIFNeuron::reset(SimInfo &info)
{
	vm = v_init;
	refrac_step = -1;
	i_syn = 0;
	return init(info.dt);
}

Type LIFNeuron::getType()
{
	return type;
}

real LIFNeuron::get_vm()
{
	return vm;
}

void LIFNeuron::monitor(SimInfo &info) 
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Neuron_%s.log", getID().getInfo().c_str());
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}

			fprintf(file, "C1: %f, C2: %f\n", C1, C2);
			fprintf(file, "i_offset: %f, vrest: %f\n", i_offset, v_rest);
			fprintf(file, "T_ref: %f\n", tau_refrac);
			fprintf(file, "vm = %f, v_thresh:%f, v_reset:%f\n", vm, v_thresh, v_reset);
		}
		fprintf(file, "Cycle %d: vm = %f\n", info.currCycle, vm); 
	}
}

size_t LIFNeuron::getSize()
{
	return sizeof(GLIFNeurons);
}

int LIFNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;

	(*p)["id"] = getID().getId();
	(*p)["v_init"] = v_init;
	(*p)["v_rest"] = v_rest;
	(*p)["v_reset"] = v_reset;
	(*p)["cm"] = cm;
	(*p)["tau_m"] = tau_m;
	(*p)["tau_refrac"] = tau_refrac;
	//(*p)["tau_syn_E"] = tau_syn_E;
	//(*p)["tau_syn_I"] = tau_syn_I;
	(*p)["v_thresh"] = v_thresh;
	(*p)["i_offset"] = i_offset;

	return 0;
}

int LIFNeuron::hardCopy(void *data, int idx, int base)
{
	GLIFNeurons * p = (GLIFNeurons *) data;
	setID(idx+base);
	p->p_v_reset[idx] = v_reset;
	p->p_v_thresh[idx] = v_thresh;
	p->p_i_syn[idx] = i_syn;
	p->p_vm[idx] = vm;
	p->p_C1[idx] = C1;
	p->p_C2[idx] = C2;
	p->p_i_tmp[idx] = i_tmp;
	p->p_refrac_step[idx] = refrac_step;
	p->p_refrac_time[idx] = refrac_time;
	
	return 1;
}
