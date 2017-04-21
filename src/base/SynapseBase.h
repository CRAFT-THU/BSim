/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "../base/NeuronBase.h"

class SynapseBase : public Base {
public:
	SynapseBase(int node = 0, real weight = 0, int delay = 0) : Base(node), _p_src(NULL), _p_dst(NULL), _weight(weight), _delay_steps(delay), _idx_dst(-1) {}
	virtual ~SynapseBase() {};

	virtual int recv() = 0;

	inline void setSrc(NeuronBase *p) {
		this->_p_src = p;
	}

	inline void setDst(NeuronBase *p) {
		this->_p_dst = p;
	}

	inline void setDst(int idx) {
		this->_idx_dst = idx;
	}

	inline NeuronBase* getSrc() {
		return _p_src;
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
	NeuronBase * _p_src;
	NeuronBase * _p_dst;
	real _weight;
	int _delay_steps;
	int _idx_dst;
	bool monitored;
};

class Greater {
	bool operator()(SynapseBase *a, SynapseBase *b) const {
		return (a->getDelay()) > (b->getDelay());
	}
};

#endif /* SYNAPSEBASE_H */

