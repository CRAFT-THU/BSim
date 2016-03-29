/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLENENGOSIMULATOR_H
#define SINGLENENGOSIMULATOR_H

#include "NengoNetwork.h"

class SingleNengoSimulator {
public:
	SingleNengoSimulator(NengoNetwork *network, real dt);
	~SingleNengoSimulator();
	virtual int run(real time);

	NengoNetwork *network;
	real dt;
};

#endif /* SINGLENENGOSIMULATOR_H */

