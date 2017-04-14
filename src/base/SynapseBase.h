/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "../base/NeuronBase.h"

class SynapseBase : public Base {
public:
	SynapseBase(int node = 0, real weight = 0, int delay = 0) : Base(node), _weight(weight), _delay_steps(delay), _p_dst(NULL) {}
	virtual ~SynapseBase() {};

	virtual int recv() = 0;

	inline void setDst(NeuronBase *p) {
		this->_p_dst = p;
	}

	inline NeuronBase* getDst() {
		return _p_dst;
	}

	inline int getDelay() {
		return _delay_steps;
	}

	inline real getWeight() {
		return _weight;
	}

	virtual void monitorOn() {
		monitored = true;
	}

protected:
	real _weight;
	int _delay_steps;
	NeuronBase * _p_dst;
	bool monitored;
};

class Greater {
	bool operator()(SynapseBase *a, SynapseBase *b) const {
		return (a->getDelay()) > (b->getDelay());
	}
};

#endif /* SYNAPSEBASE_H */

