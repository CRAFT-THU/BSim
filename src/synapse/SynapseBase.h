/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "../neuron/NeuronBase.h"

class SynapseBase : public Base {
public:
	SynapseBase();
	virtual ~SynapseBase() = 0;

	virtual int recv() = 0;
	virtual void monitorOn() = 0;
	virtual void setDst(NeuronBase *p) = 0;
	virtual int getDelay();
protected:
	real weight;
	int delay_steps;
	bool monitored;
};

class Greater {
	bool operator()(SynapseBase *a, SynapseBase *b) const {
		return (a->getDelay()) > (b->getDelay());
	}
};

#endif /* SYNAPSEBASE_H */

