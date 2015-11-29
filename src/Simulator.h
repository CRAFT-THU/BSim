/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Network.h"

class Simulator {
public:
	Simulator(Network* network, real dt);
	~Simulator();
	int run(real time);
	int reset();
protected:
	Network* network;
	real dt;
};

#endif /* SIMULATOR_H */

