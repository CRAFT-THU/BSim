
#include <math.h>

#include "../third_party/json/json.h"
#include "LIFCurrExpNeuron.h"
#include "GLIFCurrExpNeurons.h"

const Type LIFCurrExpNeuron::type = LIFCurrExp;

LIFCurrExpNeuron::LIFCurrExpNeuron(ID id, real i_offset, real i_injection, real cm, real tau_m, real tau_e, real tau_i, real vm, real v_thresh, real v_reset, real v_reset, real i_E, real i_I, int refrac_time, int refrac_step)
	: NeuronBase(id), _i_offset(i_offset), _i_injection(i_injection), _cm(cm), _tau_m(tau_m), _tau_e(tau_e), _tau_i(tau_i), _vm(vm), _v_thresh(v_thresh), _v_reset(v_reset), _v_reset(v_reset), _i_E(i_E), _i_I(i_I), _refrac_time(refrac_time), _refrac_step(refrac_step)
{
	this->monitored = false;
}

LIFCurrExpNeuron::LIFCurrExpNeuron(const LIFCurrExpNeuron &neuron, ID id) : NeuronBase(id)
{
	this->_i_offset = neuron._i_offset;
	this->_i_injection = neuron._i_injection;
	this->_cm = neuron._cm;
	this->_tau_m = neuron._tau_m;
	this->_tau_e = neuron._tau_e;
	this->_tau_i = neuron._tau_i;
	this->_vm = neuron._vm;
	this->_v_thresh = neuron._v_thresh;
	this->_v_reset = neuron._v_reset;
	this->_v_reset = neuron._v_reset;
	this->_i_E = neuron._i_E;
	this->_i_I = neuron._i_I;
	this->_refrac_time = neuron._refrac_time;
	this->_refrac_step = neuron._refrac_step;
	this->monitored = false;
}

LIFCurrExpNeuron::~LIFCurrExpNeuron()
{
	delay_queue.clear();
}

int LIFCurrExpNeuron::recv(real I)
{
}

Type LIFCurrExpNeuron::getType()
{
	return type;
}

int LIFCurrExpNeuron::reset(SimInfo &info)
{
}

int LIFCurrExpNeuron::update(SimInfo &info)
{
}

void LIFCurrExpNeuron::monitor(SimInfo &info)
{
}

size_t LIFCurrExpNeuron::getSize()
{
	return sizeof(GLIFCurrExpNeurons);
}

int LIFCurrExpNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID().getId();
	(*p)["i_offset"] = _i_offset;
	(*p)["i_injection"] = _i_injection;
	(*p)["cm"] = _cm;
	(*p)["tau_m"] = _tau_m;
	(*p)["tau_e"] = _tau_e;
	(*p)["tau_i"] = _tau_i;
	(*p)["vm"] = _vm;
	(*p)["v_thresh"] = _v_thresh;
	(*p)["v_reset"] = _v_reset;
	(*p)["v_reset"] = _v_reset;
	(*p)["i_E"] = _i_E;
	(*p)["i_I"] = _i_I;
	(*p)["refrac_time"] = _refrac_time;
	(*p)["refrac_step"] = _refrac_step;

	return 0;
}

int LIFCurrExpNeuron::hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GLIFCurrExpNeurons *p = (GLIFCurrExpNeurons *) data;
	id2idx[getID()] = idx + base;
	setIdx(idx+base);
	idx2id[idx+base] = getID();
	p->p_i_offset[idx] = _i_offset;
	p->p_i_injection[idx] = _i_injection;
	p->p_cm[idx] = _cm;
	p->p_tau_m[idx] = _tau_m;
	p->p_tau_e[idx] = _tau_e;
	p->p_tau_i[idx] = _tau_i;
	p->p_vm[idx] = _vm;
	p->p_v_thresh[idx] = _v_thresh;
	p->p_v_reset[idx] = _v_reset;
	p->p_v_reset[idx] = _v_reset;
	p->p_i_E[idx] = _i_E;
	p->p_i_I[idx] = _i_I;
	p->p_refrac_time[idx] = _refrac_time;
	p->p_refrac_step[idx] = _refrac_step;

	return 1;
}

