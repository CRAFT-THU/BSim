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

GNetwork * Simulator::getNet()
{
	SimInfo info(_dt);
	reset();
    return _network->buildNetwork(info);
}

int Simulator::reset()
{
	SimInfo info(_dt);
	_network->reset(info);

	return 0;
}

int Simulator::run (real time) {
	FireInfo fire_info;
	ArrayInfo zero_array = {0, NULL};
	fire_info["count"] = zero_array;
	int ret = run(time, fire_info);
	logFireInfo(fire_info, "count", "GFire");

	return ret;
}

#endif /* SIMULATOR_H */
