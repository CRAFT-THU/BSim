/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include "IDPool.h"

IDPool sidPool;
IDPool nidPool;

IDPool::IDPool()
{
	this->id.id = 0;
}

ID IDPool::getID()
{
	ID tmp = id;
	id.id++;
	return tmp;
}

IDPool::~IDPool()
{
}
