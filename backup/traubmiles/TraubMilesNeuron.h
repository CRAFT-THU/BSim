
#ifndef TRAUBMILESNEURON_H
#define TRAUBMILESNEURON_H

#include <stdio.h>

#include "../../interface/Neuron.h"

struct TraubMilesNeuron : Neuron 
{
public:
TraubMilesNeuron(real gNa=0, real ENa=0, real gK=0, real EK=0, real gl=0, real El=0, real C=0, real V=0, real m=0, real h=0, real n=0, real tau=1e-4, real E=0);
	TraubMilesNeuron(const TraubMilesNeuron &templ);
	~TraubMilesNeuron();

	virtual Type getType() const override;
	virtual int hardCopy(void * data, int idx, int base, const SimInfo &info) override;
	virtual Synapse * createSynapse(real weight, real delay, SpikeType type, real tau) override;

	const static Type type;

protected:
	real _gNa;
	real _ENa;
	real _gK;
	real _EK;
	real _gl;
	real _El;
	real _C;
	real _V;
	real _m;
	real _h;
	real _n;
	real _tau;
	real _E;
};
#endif /* TRAUBMILESNEURON_H */
