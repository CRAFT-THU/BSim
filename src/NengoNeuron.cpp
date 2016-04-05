/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "utils/json/json.h"
#include "NengoNeuron.h"

using std::vector;

const Type NengoNeuron::type = Nengo;

NengoNeuron::NengoNeuron(ID id, real v_init, real v_min, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset, real encoder)
	:v_init(v_init), v_min(v_min), v_reset(v_reset), cm(cm), tau_m(tau_m), tau_refrac(tau_refrac), tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I), v_thresh(v_thresh), i_offset(i_offset), encoder(encoder)
{
	this->i_syn = 0;
	this->id = id;
	this->file = NULL;
	this->monitored = false;
}

NengoNeuron::NengoNeuron(const NengoNeuron &neuron, ID id)
{
	this->v_init = neuron.v_init;
	this->v_min = neuron.v_min;
	this->v_reset = neuron.v_reset;
	this->cm = neuron.cm;
	this->tau_m = neuron.tau_m;
	this->tau_refrac = neuron.tau_refrac;
	this->tau_syn_E = neuron.tau_syn_E;
	this->tau_syn_I = neuron.tau_syn_I;
	this->v_thresh = neuron.v_thresh;
	this->i_offset = neuron.i_offset;
	this->encoder = neuron.encoder;
	this->i_syn = 0;
	this->file = NULL;
	this->id = id;
}

NengoNeuron::~NengoNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int NengoNeuron::init(real dt)
{
	_dt = dt;
	if (tau_m > 0) {
		C1 = -expm1(-dt/tau_m);
	} else {
		C1 = 1;
	}

	i_tmp = i_offset;

	refrac_time = 0;

	return 0;
}

int NengoNeuron::update(SimInfo &info)
{
	fired = false;

	//real I = i_syn * encoder + i_tmp;
	real I = i_syn + i_tmp;
	real dv = C1 * (I - vm);
	vm = vm + dv;

	real vm_t = vm;

	if (vm < v_min) {
		vm = v_min;
	}

	refrac_time -= _dt;

	real tmp = 1-refrac_time/_dt;
	if (tmp < 0) {
		tmp = 0;
	}
	if (tmp > 1) {
		tmp = 1;
	}

	vm = vm * tmp;

	if (file != NULL) {
		fprintf(file, "Cycle %d: i_syn=%f i=%f vm = %f vm' = %f\n", info.currCycle, i_syn, I, vm, vm_t); 
	} else {
			char filename[128];
			sprintf(filename, "Neuron_%d_%d.log", id.groupId, id.id);
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return -1;
			}

			fprintf(file, "C1: %f\n", C1);
			fprintf(file, "i_offset: %f, v_min: %f, encoder: %f\n", i_offset, v_min, encoder);
			fprintf(file, "T_ref: %f, dt: %f\n", tau_refrac, _dt);
			fprintf(file, "vm = %f, v_thresh:%f, v_reset:%f\n", vm, v_thresh, v_reset);
			fprintf(file, "Cycle %d: i_syn=%f i=%f vm = %f vm' = %f\n", info.currCycle, i_syn, I, vm, vm_t); 
	}

	if (vm >= v_thresh) {
		real overshoot = (vm -1)/dv;
		real spiketime = _dt*(1-overshoot);
		fired = true;
		refrac_time = tau_refrac + spiketime;
		vm = 0;
	}

	i_syn = 0;

	if (fired) {
	//TODO fire
		fire();
		info.fired.push_back(this->id);
		return 1;
	} else {
		return -1;
	}
}

int NengoNeuron::recv(real I)
{
	i_syn += I;
	//printf("Neuron: %d_%d, %f\n", this->getID().groupId, this->getID().id, i_syn);
	//getchar();

	return 0;
}

int NengoNeuron::reset(SimInfo &info)
{
	vm = v_init;
	refrac_time = -1;
	i_syn = 0;
	return init(info.dt);
}

Type NengoNeuron::getType()
{
	return type;
}

real NengoNeuron::get_vm()
{
	return vm;
}

void NengoNeuron::monitor(SimInfo &info) 
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Neuron_%d_%d.log", id.groupId, id.id);
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}

			fprintf(file, "C1: %f\n", C1);
			fprintf(file, "i_offset: %f, v_min: %f, encoder: %f\n", i_offset, v_min, encoder);
			fprintf(file, "T_ref: %f, dt: %f\n", tau_refrac, _dt);
			fprintf(file, "vm = %f, v_thresh:%f, v_reset:%f\n", vm, v_thresh, v_reset);
		}
		//fprintf(file, "Cycle %d: vm = %f\n", info.currCycle, vm); 
	}
}

size_t NengoNeuron::getSize()
{
	//return sizeof(GNengoNeurons);
	return 0;
}

int NengoNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;

	(*p)["id"] = id.id;
	(*p)["v_init"] = v_init;
	(*p)["v_min"] = v_min;
	(*p)["v_reset"] = v_reset;
	(*p)["cm"] = cm;
	(*p)["tau_m"] = tau_m;
	(*p)["tau_refrac"] = tau_refrac;
	(*p)["tau_syn_E"] = tau_syn_E;
	(*p)["tau_syn_I"] = tau_syn_I;
	(*p)["v_thresh"] = v_thresh;
	(*p)["i_offset"] = i_offset;

	return 0;
}

int NengoNeuron::hardCopy(void *data, int idx)
{
	//GNengoNeurons * p = (GNengoNeurons *) data;
	//p->pID[idx] = id;
	//p->pType[idx] = type;
	//p->p_v_init[idx] = v_init;
	//p->p_v_rest[idx] = v_rest;
	//p->p_v_reset[idx] = v_reset;
	//p->p_cm[idx] = cm;
	//p->p_tau_m[idx] = tau_m;
	//p->p_tau_refrac[idx] = tau_refrac;
	//p->p_tau_syn_E[idx] = tau_syn_E;
	//p->p_tau_syn_I[idx] = tau_syn_I;
	//p->p_v_thresh[idx] = v_thresh;
	//p->p_i_offset[idx] = i_offset;
	//p->p_i_syn[idx] = i_syn;
	//p->p_vm[idx] = vm;
	//p->p__dt[idx] = _dt;
	//p->p_C1[idx] = C1;
	//p->p_C2[idx] = C2;
	//p->p_i_tmp[idx] = i_tmp;
	//p->p_refrac_step[idx] = refrac_step;
	
	return 1;
}

int NengoNeuron::fire()
{
	printf("ERROR: Should NOT RUN\n");
	return 0;
}	

SynapseBase* NengoNeuron::addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest)
{
	printf("ERROR: Should NOT RUN\n");
	return NULL;
}
