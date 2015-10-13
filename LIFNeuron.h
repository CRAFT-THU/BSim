/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef LIFNEURON_H
#define LIFNEURON_H
#include "Neuron.h"

class LIFNeuron : Neuron {
public:
	LIFNeuron(int _N, double *_synapsis, double _tm, double _thV = 1.0, double _rV = 0.0, double axon = 0.0);
	~LIFNeuron();

	void presynapsis();
	void integrate();
	void postsynapsis();
private:
	double alpha;
	double tm;
	double leak;
};

#endif /* LIFNEURON_H */

