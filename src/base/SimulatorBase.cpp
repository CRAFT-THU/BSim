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

int SimulatorBase::reset()
{
	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	network->reset(info);

	return 0;
}

int SimulatorBase::run (real time) {
	FireInfo tmp;
	return run(time, tmp);
}
