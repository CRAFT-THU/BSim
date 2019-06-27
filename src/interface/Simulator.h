/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "../utils/SimInfo.h"
#include "../net/Network.h"

class Simulator {
public:
	Simulator(Network* network, real dt) : _network(network), _dt(dt) {}
	virtual ~Simulator() {}

	virtual int run(real time, FireInfo &log) = 0;
	virtual int reset();
    GNetwork * getNet();

	int run(real time);
protected:
	Network* _network;
	real _dt;
};

#endif /* SIMULATOR_H */
