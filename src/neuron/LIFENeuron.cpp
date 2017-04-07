
#include "math.h"

#include "../third_party/json/json.h"
#include "LIFENeuron.h"
#include "GLIFENeurons.h"

const Type LIFENeuron::type = LIFE;

LIFENeuron::LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	: NeuronBase(id), _v_thresh(v_thresh), _v_reset(v_reset), _v_init(v_init), _v_rest(v_rest), _cm(cm), _tau_m(tau_m), _tau_refrac(tau_refrac), _tau_syn_E(tau_syn_E), _tau_syn_I(tau_syn_I), _i_offset(i_offset)
{
	this->_i_syn_E = 0.0f;
	this->_i_syn_I = 0.0f;
	this->monitored = false;
}

LIFENeuron::LIFENeuron(const LIFENeuron &neuron, ID id) : NeuronBase(id)
{
	this->_CI = neuron._CI;
	this->_vm = neuron._vm;
	this->_CE = neuron._CE;
	this->_refrac_step = neuron._refrac_step;
	this->_refrac_time = neuron._refrac_time;
	this->_i_tmp = neuron._i_tmp;
	this->_i_I = neuron._i_I;
	this->_i_E = neuron._i_E;
	this->_v_thresh = neuron._v_thresh;
	this->_v_reset = neuron._v_reset;
	this->_C2 = neuron._C2;
	this->_C1 = neuron._C1;
	this->monitored = false;

	this->_v_init = neuron._v_init;
	this->_v_rest = neuron._v_rest;
	this->_cm = neuron._cm;
	this->_tau_m = neuron._tau_m;
	this->_tau_refrac = neuron._tau_refrac;
	this->_tau_syn_E = neuron._tau_syn_E;
	this->_tau_syn_I = neuron._tau_syn_I;
	this->_i_offset = neuron._i_offset;

	this->_i_syn_E = 0.0f;
	this->_i_syn_I = 0.0f;
}

LIFENeuron::~LIFENeuron()
{
}

int LIFENeuron::recv(real I)
{
	if ( I >= 0 ) {
		_i_syn_E += I;
	} else {
		_i_syn_I += I;
	}

	return 0;
}

Type LIFENeuron::getType()
{
	return type;
}

int LIFENeuron::reset(SimInfo &info)
{
	real dt = info.dt;

	real rm = 1.0;
	if (fabs(_cm) > ZERO) {
		rm = _tau_m/_cm;
	}
	if (_tau_m > 0) {
		_C1 = exp(-dt/_tau_m);
		_C2 = rm*(1-_C1);
	} else {
		_C1 = 0.0f;
		_C2 = rm;
	}

	if (_tau_syn_E > 0) {
		_CE = exp(-dt/_tau_syn_E);
	} else {
		_CE = 0.0f;
	}

	if (_tau_syn_I > 0) {
		_CI = exp(-dt/_tau_syn_I);
	} else {
		_CI = 0.0f;
	}

	if (rm > 0) {
		_i_tmp = _i_offset + _v_rest/rm;
	} else {
		_i_tmp = 0;
	}

	_refrac_time = static_cast<int>(_tau_refrac/dt);
	_refrac_step = _refrac_time;

	_i_I = 0;
	_i_E = 0;

	_vm = _v_init;

	return 0;
}

int LIFENeuron::update(SimInfo &info)
{
	fired = false;

	if (_refrac_step > 0) {
		--_refrac_step;
	} else {
		real I = _i_E * sqrtf(_CE) + _i_I * sqrtf(_CI) + _i_tmp;
		_vm = _C1 * _vm + _C2 * I;

		_i_E = _CE * _i_E;
		_i_I = _CI * _i_I;
		
		if (_vm >= _v_thresh) {
			fired = true;
			//refrac_step = (int)(tau_refrac/_dt) - 1;
			_refrac_step = _refrac_time - 1;
			_vm = _v_reset;
		} else {
			_i_E += _i_syn_E;
			_i_I += _i_syn_I;
		}
	}
	
	info.input.push_back(_vm);
	info.input.push_back(_i_E);
	info.input.push_back(_i_I);
	info.input.push_back(_i_E * sqrtf(_CE) + _i_I * sqrtf(_CI));


	_i_syn_E = 0;
	_i_syn_I = 0;

	if (fired) {
		//TODO fire
		fire();
		info.fired.push_back(getID());
		return 1;
	} else {
		return -1;
	}
}

void LIFENeuron::monitor(SimInfo &info)
{
}

size_t LIFENeuron::getSize()
{
	return sizeof(GLIFENeurons);
}

int LIFENeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID().getId();
	(*p)["CI"] = _CI;
	(*p)["vm"] = _vm;
	(*p)["CE"] = _CE;
	(*p)["refrac_step"] = _refrac_step;
	(*p)["refrac_time"] = _refrac_time;
	(*p)["i_tmp"] = _i_tmp;
	(*p)["i_I"] = _i_I;
	(*p)["i_E"] = _i_E;
	(*p)["v_thresh"] = _v_thresh;
	(*p)["v_reset"] = _v_reset;
	(*p)["C2"] = _C2;
	(*p)["C1"] = _C1;

	return 0;
}

int LIFENeuron::hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GLIFENeurons *p = (GLIFENeurons *) data;
	id2idx[getID()] = idx + base;
	setIdx(idx+base);
	idx2id[idx+base] = getID();
	p->p_CI[idx] = _CI;
	p->p_vm[idx] = _vm;
	p->p_CE[idx] = _CE;
	p->p_refrac_step[idx] = _refrac_step;
	p->p_refrac_time[idx] = _refrac_time;
	p->p_i_tmp[idx] = _i_tmp;
	p->p_i_I[idx] = _i_I;
	p->p_i_E[idx] = _i_E;
	p->p_v_thresh[idx] = _v_thresh;
	p->p_v_reset[idx] = _v_reset;
	p->p_C2[idx] = _C2;
	p->p_C1[idx] = _C1;

	return 1;
}

