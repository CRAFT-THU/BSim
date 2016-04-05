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
	SimInfo info = { 0, dt };

	network->reset(info);

	return 0;
}
