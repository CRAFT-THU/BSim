/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include "SynapseBase.h"

SynapseBase::SynapseBase() 
{
	monitored = false;
}

SynapseBase::~SynapseBase() {}

int SynapseBase::setNode(int node)
{
	this->id.setNode(node);
	return 1;
}

void SynapseBase::monitorOn()
{
	monitored = false;
}

int SynapseBase::getDelay()
{
	return this->delay_steps;
}
