/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef NEURONBASE_H
#define NEURONBASE_H

#include "constant.h"
#include "Base.h"

class SynapseBase;

class NeuronBase : public Base {
public:
	virtual ~NeuronBase() = 0;

	virtual int fire() = 0;
	virtual bool isFired() = 0;

	virtual void monitorOn() = 0;

	virtual int recv(real I) = 0;
	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, NeuronBase *dst) = 0;
};

#endif /* NEURONBASE_H */

