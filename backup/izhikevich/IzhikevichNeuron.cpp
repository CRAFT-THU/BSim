#include <math.h>

#include "IzhikevichNeuron.h"
#include "IzhikevichData.h"

const Type IzhikevichNeuron::type = Izhikevich;

IzhikevichNeuron::IzhikevichNeuron(real v, real u, real a, real b, real c, real d): Neuron(), _v(v), _u(u), _a(a), _b(b), _c(c), _d(d)
{
}

IzhikevichNeuron::IzhikevichNeuron(const IzhikevichNeuron &templ)
{
	this->_v = templ._v;
	this->_u = templ._u;
	this->_a = templ._a;
	this->_b = templ._b;
	this->_c = templ._c;
	this->_d = templ._d;
}

IzhikevichNeuron::~IzhikevichNeuron()
{
}

Type IzhikevichNeuron::getType() const
{
	return type;
}


Synapse * IzhikevichNeuron::createSynapse(real weight, real delay, SpikeType type, real tau)
{
	printf("Not implemented!\n");
	return NULL;
}


int IzhikevichNeuron::hardCopy(void * data, int idx, int base, const SimInfo &info)
{
	IzhikevichData *p = (IzhikevichData *)data;

	setID(idx+base);

	p->pV[idx] = this->_v;
	p->pU[idx] = this->_u;
	p->pA[idx] = this->_a;
	p->pB[idx] = this->_b;
	p->pC[idx] = this->_c;
	p->pD[idx] = this->_d;
	return 1;
}

