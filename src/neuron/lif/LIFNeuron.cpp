
#include "math.h"

#include "../../third_party/json/json.h"
#include "LIFNeuron.h"
#include "GLIFNeurons.h"

const Type LIFNeuron::type = LIF;

LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	: NeuronBase(),_v_init(v_init), _v_rest(v_rest), _v_reset(v_reset), _cm(cm), _tau_m(tau_m), _tau_refrac(tau_refrac), _tau_syn_E(tau_syn_E), _tau_syn_I(tau_syn_I), _v_thresh(v_thresh), _i_offset(i_offset)
{
	// this->_i_syn_E = 0.0;
	// this->_i_syn_I = 0.0;
	this->monitored = false;
}

LIFNeuron::LIFNeuron(const LIFNeuron &neuron) : NeuronBase()
{
	// this->_vm = neuron._vm;
	// this->_CI = neuron._CI;
	// this->_CE = neuron._CE;
	// this->_C_I = neuron._C_I;
	// this->_C_E = neuron._C_E;
	// this->_refrac_step = neuron._refrac_step;
	// this->_refrac_time = neuron._refrac_time;
	// this->_v_tmp = neuron._v_tmp;
	// this->_i_I = neuron._i_I;
	// this->_i_E = neuron._i_E;
	// this->_Cm = neuron._Cm;

	this->_v_init = neuron._v_init;
	this->_v_rest = neuron._v_rest;
	this->_v_reset = neuron._v_reset;
	this->_cm = neuron._cm;
	this->_tau_m = neuron._tau_m;
	this->_tau_refrac = neuron._tau_refrac;
	this->_tau_syn_E = neuron._tau_syn_E;
	this->_tau_syn_I = neuron._tau_syn_I;
	this->_v_thresh = neuron._v_thresh;
	this->_i_offset = neuron._i_offset;

	// this->_i_syn_E = 0.0;
	// this->_i_syn_I = 0.0;
	this->monitored = false;
}

LIFNeuron::~LIFNeuron()
{
}

Type LIFNeuron::getType()
{
	return type;
}

int LIFNeuron::reset(SimInfo &info)
{
	// real dt = info.dt;

	// real rm = 1.0;
	// if (fabs(_cm) > ZERO) {
	// 	rm = _tau_m/_cm;
	// }
	// if (_tau_m > 0) {
	// 	_Cm = exp(-dt/_tau_m);
	// } else {
	// 	_Cm = 0.0;
	// }

	// if (_tau_syn_E > 0) {
	// 	_CE = exp(-dt/_tau_syn_E);
	// } else {
	// 	_CE = 0.0;
	// }

	// if (_tau_syn_I > 0) {
	// 	_CI = exp(-dt/_tau_syn_I);
	// } else {
	// 	_CI = 0.0;
	// }

	// _v_tmp = _i_offset * rm + _v_rest;
	// _v_tmp *= (1-_Cm);

	// _C_E = rm * _tau_syn_E/( _tau_syn_E - _tau_m);
	// _C_I = rm * _tau_syn_I/( _tau_syn_I - _tau_m);

	// _C_E = _C_E * (_CE - _Cm);
	// _C_I = _C_I * (_CI - _Cm);

	// _refrac_time = static_cast<int>(_tau_refrac/dt);
	// _refrac_step = 0;

	// _i_I = 0;
	// _i_E = 0;

	// _vm = _v_init;

	// return 0;
}

void LIFNeuron::monitor(SimInfo &info)
{
}

size_t LIFNeuron::getSize()
{
	return sizeof(GLIFNeurons);
}

int LIFNeuron::hardCopy(void * data, int idx, int base, SimInfo &info)
{
	GLIFNeurons *p = (GLIFNeurons *) data;

	real dt = info.dt;
	real rm = (fabs(_cm) > ZERO)?(_tau_m/_cm):1.0;
	real Cm = (_tau_m>0)?exp(-dt/_tau_m):0.0;
	real Ce = (_tau_syn_E > 0)?exp(-dt/_tau_syn_E):0.0;
	real Ci = (_tau_syn_I > 0)?exp(-dt/_tau_syn_I):0.0;

	real v_tmp = _i_offset * rm + _v_rest;
	v_tmp *= (1-Cm);

	real C_e = rm * _tau_syn_E/( _tau_syn_E - _tau_m);
	real C_i = rm * _tau_syn_I/( _tau_syn_I - _tau_m);

	C_e = C_e * (Ce - Cm);
	C_i = C_i * (Ci - Cm);

	setID(idx+base);
	p->pV_m[idx] = _v_init;
	p->pCi[idx] = Ci;
	p->pCe[idx] = Ce;
	p->pC_i[idx] = C_i;
	p->pC_e[idx] = C_e;
	p->pV_tmp[idx] = v_tmp;
	p->pI_i[idx] = 0;
	p->pI_e[idx] = 0;
	p->pV_thresh[idx] = _v_thresh;
	p->pV_reset[idx] = _v_reset;
	p->pC_m[idx] = Cm;

	p->pRefracStep[idx] = 0;
	p->pRefracTime[idx] = static_cast<int>(_tau_refrac/dt);;
	//p->p_start_I[idx] = this->_start_I;
	//p->p_start_E[idx] = this->_start_E;
	//p->p_end[idx] = this->_end;
	return 1;
}

