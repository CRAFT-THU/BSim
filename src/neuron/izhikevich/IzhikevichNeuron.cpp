#include <math.h>

#include "IzhikevichNeuron.h"
#include "IzhikevichData.h"

	const Type IzhikevichNeuron::type = IZHIKEVICH;

IzhikevichNeuron::IzhikevichNeuron(real vreal ureal areal breal creal d): Neuron(), _v(v), _u(u), _a(a), _b(b), _c(c), _d(d)
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

Type {}::getType() const
{
	return type;
}

Synapse * IzhikevichNeuron::createSynapse(real weight, real delay, SpikeType type, real tau)
{
	printf("Not implemented!\n");
	return NULL;
}

int {}::hardCopy(void * data, int idx, int base, const SimInfo &info)
{
	IzhikevichData *p = (Data *)data;
	real dt = info.dt;
	int delay_steps = static_cast<int>(round(_delay/dt));

	setID(idx+base);

	p->pV = this->_v;
	p->pU = this->_u;
	p->pA = this->_a;
	p->pB = this->_b;
	p->pC = this->_c;
	p->pD = this->_d;
	return 1;
}
