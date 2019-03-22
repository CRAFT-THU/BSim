
#include "math.h"

#include "../../third_party/json/json.h"
#include "LIFNeuron.h"
#include "GLIFNeurons.h"

const Type LIFNeuron::type = LIF;

LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	: NeuronBase(), _v_thresh(v_thresh), _v_reset(v_reset), _v_init(v_init), _v_rest(v_rest), _cm(cm), _tau_m(tau_m), _tau_refrac(tau_refrac), _tau_syn_E(tau_syn_E), _tau_syn_I(tau_syn_I), _i_offset(i_offset)
{
	this->_i_syn_E = 0.0;
	this->_i_syn_I = 0.0;
	this->monitored = false;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron) : NeuronBase()
{
	this->_vm = neuron._vm;
	this->_CI = neuron._CI;
	this->_CE = neuron._CE;
	this->_C_I = neuron._C_I;
	this->_C_E = neuron._C_E;
	this->_refrac_step = neuron._refrac_step;
	this->_refrac_time = neuron._refrac_time;
	this->_v_tmp = neuron._v_tmp;
	this->_i_I = neuron._i_I;
	this->_i_E = neuron._i_E;
	this->_v_thresh = neuron._v_thresh;
	this->_v_reset = neuron._v_reset;
	this->_Cm = neuron._Cm;
	this->monitored = false;

	this->_v_init = neuron._v_init;
	this->_v_rest = neuron._v_rest;
	this->_cm = neuron._cm;
	this->_tau_m = neuron._tau_m;
	this->_tau_refrac = neuron._tau_refrac;
	this->_tau_syn_E = neuron._tau_syn_E;
	this->_tau_syn_I = neuron._tau_syn_I;
	this->_i_offset = neuron._i_offset;

	this->_i_syn_E = 0.0;
	this->_i_syn_I = 0.0;
}

LIFNeuron::~LIFNeuron()
{
}

int LIFNeuron::recv(real I)
{
	if ( I >= 0 ) {
		_i_syn_E += I;
	} else {
		_i_syn_I += I;
	}

	return 0;
}

Type LIFNeuron::getType()
{
	return type;
}

int LIFNeuron::reset(SimInfo &info)
{
	real dt = info.dt;

	real rm = 1.0;
	if (fabs(_cm) > ZERO) {
		rm = _tau_m/_cm;
	}
	if (_tau_m > 0) {
		_Cm = exp(-dt/_tau_m);
	} else {
		_Cm = 0.0;
	}

	if (_tau_syn_E > 0) {
		_CE = exp(-dt/_tau_syn_E);
	} else {
		_CE = 0.0;
	}

	if (_tau_syn_I > 0) {
		_CI = exp(-dt/_tau_syn_I);
	} else {
		_CI = 0.0;
	}

	_v_tmp = _i_offset * rm + _v_rest;
	_v_tmp *= (1-_Cm);

	_C_E = rm * _tau_syn_E/( _tau_syn_E - _tau_m);
	_C_I = rm * _tau_syn_I/( _tau_syn_I - _tau_m);

	_C_E = _C_E * (_CE - _Cm);
	_C_I = _C_I * (_CI - _Cm);

	_refrac_time = static_cast<int>(_tau_refrac/dt);
	_refrac_step = 0;

	_i_I = 0;
	_i_E = 0;

	_vm = _v_init;

	return 0;
}

int LIFNeuron::update(SimInfo &info)
{
	fired = false;

	if (_refrac_step > 0) {
		--_refrac_step;
	} else {
		_vm = _Cm * _vm + _v_tmp + _i_E * _C_E + _i_I * _C_I;

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

		if (fired) {
			fire();
			fire_count++;
		} else {
			input_current += _i_syn_E + _i_syn_I;
		}
	}
	
	info.input.push_back(_vm);
	info.input.push_back(_i_syn_E);
	info.input.push_back(_i_syn_I);
	info.input.push_back(_i_E);
	info.input.push_back(_i_I);
	//info.input.push_back(_i_E + _i_I);


	_i_syn_E = 0;
	_i_syn_I = 0;

	info.fired.push_back(fired);

	return fired;


}

void LIFNeuron::monitor(SimInfo &info)
{
}

size_t LIFNeuron::getSize()
{
	return sizeof(GLIFNeurons);
}

int LIFNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID();
	(*p)["vm"] = _vm;
	(*p)["CI"] = _CI;
	(*p)["CE"] = _CE;
	(*p)["C_I"] = _C_I;
	(*p)["C_E"] = _C_E;
	(*p)["refrac_step"] = _refrac_step;
	(*p)["refrac_time"] = _refrac_time;
	(*p)["v_tmp"] = _v_tmp;
	(*p)["i_I"] = _i_I;
	(*p)["i_E"] = _i_E;
	(*p)["v_thresh"] = _v_thresh;
	(*p)["v_reset"] = _v_reset;
	(*p)["Cm"] = _Cm;

	return 0;
}

int LIFNeuron::hardCopy(void * data, int idx, int base)
{
	GLIFNeurons *p = (GLIFNeurons *) data;
	setID(idx+base);
	p->p_vm[idx] = _vm;
	p->p_CI[idx] = _CI;
	p->p_CE[idx] = _CE;
	p->p_C_I[idx] = _C_I;
	p->p_C_E[idx] = _C_E;
	p->p_v_tmp[idx] = _v_tmp;
	p->p_i_I[idx] = _i_I;
	p->p_i_E[idx] = _i_E;
	p->p_v_thresh[idx] = _v_thresh;
	p->p_v_reset[idx] = _v_reset;
	p->p_Cm[idx] = _Cm;

	p->p_refrac_step[idx] = _refrac_step;
	p->p_refrac_time[idx] = _refrac_time;
	//p->p_start_I[idx] = this->_start_I;
	//p->p_start_E[idx] = this->_start_E;
	//p->p_end[idx] = this->_end;
	return 1;
}

