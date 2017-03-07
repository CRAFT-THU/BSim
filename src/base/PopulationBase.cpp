/* This program is writen by qp09.
 * usually just for fun.
 * Mon January 18 2016
 */

#include "PopulationBase.h"

PopulationBase::PopulationBase(ID id, int node) : Base(id, node)
{
	monitored = false;
}

PopulationBase::~PopulationBase() {}

void PopulationBase::monitorOn()
{
	monitored = true;
}
