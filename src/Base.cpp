/* This program is writen by qp09.
 * usually just for fun.
 * Tue April 05 2016
 */

#include "Base.h"

Base::Base()
{
}

Base::~Base()
{
}

ID Base::getID()
{
	return id;
}

void Base::setID(ID id)
{
	this->id = id;
}
