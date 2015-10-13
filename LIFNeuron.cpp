/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include "LIFNeuron.h"

LIFNeuron::LIFNeuron(int _N, double *_synapsis, double _tm, double _thV = 1.0, double _rV = 0.0, double _axon = 0.0)
{
	N = _N;
	synapsis = new double[_N*2];
	for (int i=0; i<N; i++) {
		synapsis[i*2] = _synapsis[i];
		synapsis[i*2+1] = 0.0;
	}
	tm = _tm;
	threshold = _thV;
	reset = _rV;
	axon = _axon;
	membraneV = reset;
}

LIFNeuron::~LIFNeuron()
{
	delete synapsis[];
}

LIFNeuron::void presynapsis() { return; }

LIFNeuron::void integrate()
{
	for (int i=0; i<N; i++) {
		membraneV += synapsis[i*2] * synapsis[i*2+1];
	}
}

LIFNeuron::void postsynapsis()
{
}
