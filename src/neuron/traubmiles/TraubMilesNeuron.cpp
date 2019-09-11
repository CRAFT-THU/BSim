#include <math.h>

#include "TraubMilesNeuron.h"
#include "TraubMilesData.h"

const Type TraubMilesNeuron::type = TraubMiles;

TraubMilesNeuron::TraubMilesNeuron(real gNa, real ENa, real gK, real EK, real gl, real El, real C, real V, real m, real h, real n, real tau, real E): Neuron(), _gNa(gNa), _ENa(ENa), _gK(gK), _EK(EK), _gl(gl), _El(El), _C(C), _V(V), _m(m), _h(h), _n(n), _tau(tau), _E(E)
{
}

TraubMilesNeuron::TraubMilesNeuron(const TraubMilesNeuron &templ)
{
	this->_gNa = templ._gNa;
	this->_ENa = templ._ENa;
	this->_gK = templ._gK;
	this->_EK = templ._EK;
	this->_gl = templ._gl;
	this->_El = templ._El;
	this->_C = templ._C;
	this->_V = templ._V;
	this->_m = templ._m;
	this->_h = templ._h;
	this->_n = templ._n;
	this->_tau = templ._tau;
	this->_E = templ._E;
}

TraubMilesNeuron::~TraubMilesNeuron()
{
}

Type TraubMilesNeuron::getType() const
{
	return type;
}


Synapse * TraubMilesNeuron::createSynapse(real weight, real delay, SpikeType type, real tau)
{
	printf("Not implemented!\n");
	return NULL;
}


int TraubMilesNeuron::hardCopy(void * data, int idx, int base, const SimInfo &info)
{
	TraubMilesData *p = (TraubMilesData *)data;

	setID(idx+base);

	p->pGNa[idx] = this->_gNa;
	p->pENa[idx] = this->_ENa;
	p->pGK[idx] = this->_gK;
	p->pEK[idx] = this->_EK;
	p->pGl[idx] = this->_gl;
	p->pEl[idx] = this->_El;
	p->pC[idx] = this->_C;
	p->pV[idx] = this->_V;
	p->pM[idx] = this->_m;
	p->pH[idx] = this->_h;
	p->pN[idx] = this->_n;
	p->pTau[idx] = this->_tau;
	p->pE[idx] = this->_E;
	return 1;
}

