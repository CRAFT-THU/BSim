/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Connector.h"

class Simulator {
public:
	Simulator(Connector* network, real dt);
	~Simulator();
	int run(real dt);
protected:
	Connector* network;
	real dt;
};

#endif /* SIMULATOR_H */

