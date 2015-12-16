/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATORBASE_H
#define SIMULATORBASE_H

#include "Network.h"

class SimulatorBase {
public:
	SimulatorBase(Network* network, real dt);
	virtual ~SimulatorBase();
	virtual int run(real time) = 0;
	virtual int reset();
protected:
	Network* network;
	real dt;
};

#endif /* SIMULATORBASE_H */

