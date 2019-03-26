/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATORBASE_H
#define SIMULATORBASE_H

#include "../utils/SimInfo.h"
#include "../net/Network.h"

class SimulatorBase {
public:
	SimulatorBase(Network* network, real dt);
	virtual ~SimulatorBase();

	virtual int run(real time, FireInfo &log) = 0;
	virtual int reset();

	int run(real time);
protected:
	Network* network;
	real dt;
};

#endif /* SIMULATORBASE_H */

