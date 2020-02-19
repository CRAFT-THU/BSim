
#ifndef IZHIKEVICHNEURON_H
#define IZHIKEVICHNEURON_H

#include <stdio.h>

#include "../../interface/Neuron.h"

struct IzhikevichNeuron : Neuron 
{
public:
IzhikevichNeuron(real v=0, real u=0, real a=0, real b=0, real c=0, real d=0);
	IzhikevichNeuron(const IzhikevichNeuron &templ);
	~IzhikevichNeuron();

	virtual Type getType() const override;
	virtual int hardCopy(void * data, int idx, int base, const SimInfo &info) override;
	virtual Synapse * createSynapse(real weight, real delay, SpikeType type, real tau) override;

	const static Type type;

protected:
	real _v;
	real _u;
	real _a;
	real _b;
	real _c;
	real _d;
};
#endif /* IZHIKEVICHNEURON_H */
