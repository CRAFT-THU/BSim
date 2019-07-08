/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <math.h>

#include "../interface/Neuron.h"

class Synapse: public Model {
public:
	Synapse(int node = 0, real weight = 0, real delay = 0) : Model(node)/*, _p_src(NULL)*/, _p_dst(NULL), _weight(weight), _delay(delay)/*, _idx_dst(-1)*/ {}
	virtual ~Synapse() {};

	// virtual int recv() = 0;

	inline void setSrc(Neuron *p) {
		this->_p_src = p;
	}

	inline void setDst(Neuron *p) {
		this->_p_dst = p;
	}

	//inline void setDst(int idx) {
	//	this->_idx_dst = idx;
	//}

	//inline NeuronBase* getSrc() {
	//	return _p_src;
	//}

	inline Neuron* getSrc() {
		return _p_src;
	}

	inline Neuron* getDst() {
		return _p_dst;
	}

	inline real getRealDelay() {
		return _delay;
	}

	inline int getDelaySteps(real dt) {
		return static_cast<int>(round(_delay/dt));
	}

	inline real getWeight() {
		return _weight;
	}

	virtual void monitorOn() {
		monitored = true;
	}

protected:
	Neuron *_p_src;
	Neuron *_p_dst;
	real _weight;
	real _delay;
	//int _idx_dst;
	bool monitored;
};

class Greater {
	bool operator()(Synapse *a, Synapse *b) const {
		return (a->getRealDelay()) > (b->getRealDelay());
	}
};

#endif /* SYNAPSE_H */

