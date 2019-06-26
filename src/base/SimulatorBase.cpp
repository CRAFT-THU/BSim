/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 13 2015
 */

#include "SimulatorBase.h"

SimulatorBase::SimulatorBase(Network* network, real dt)
{
	this->network = network;
	this->dt = dt;
}

SimulatorBase::~SimulatorBase()
{
}

GNetwork * SimulatorBase::getNet()
{
	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;
	reset();
    return this->network->buildNetwork(info);
}

int SimulatorBase::reset()
{
	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	network->reset(info);

	return 0;
}

int SimulatorBase::run (real time) {
	FireInfo fire_info;
	ArrayInfo zero_array = {0, NULL};
	fire_info["count"] = zero_array;
	int ret = run(time, fire_info);
	logFireInfo(fire_info, "count", "GFire");

	return ret;
}
