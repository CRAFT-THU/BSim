/* This program is writen by qp09.
 * usually just for fun.
 * Wed October 28 2015
 */
#include "ID.h"

ID::ID()
{
	id = 0;
}

ID::ID(const ID &a)
{
	this->id = a.id;
}

ID::~ID()
{
}

bool ID::operator==(const ID &id1)const{  
	return (id == id1.id);  
}    

bool ID::operator<(const ID &id1)const{  
	return (id < id1.id);  
}    
