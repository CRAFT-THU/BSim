/* This program is writen by qp09.
 * usually just for fun.
 * Wed October 28 2015
 */
#include "ID.h"

ID::ID()
{
	id = 0;
}

ID::ID(int _id)
{
	id = _id;
	groupId = -1;
}

ID::ID(int _groupId, int _id)
{
	id = _id;
	groupId = _groupId;
}

ID::ID(const ID &a)
{
	this->id = a.id;
	this->groupId = a.groupId;
}

ID::~ID()
{
}

bool ID::operator==(const ID &id1)const{  
	return ((groupId == id1.groupId) && (id == id1.id));  
}    

//bool ID::operator==(const int &id1)const{  
//	return (id == id1);  
//}    

bool ID::operator<(const ID &id1)const{  
	if (groupId < id1.groupId) {
		return true;
	} 

	if ((groupId == id1.groupId) && (id < id1.id)) {
		return true;
	}

	return false;
}    
