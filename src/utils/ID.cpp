/* This program is writen by qp09.
 * usually just for fun.
 * Wed October 28 2015
 */

#include <sstream>

#include "ID.h"

using std::stringstream;

ID::ID()
{
	this->_id.packed_id = 0;
}

ID::ID(int id)
{
	this->_id.detailed_id.grp_id = 0;
	this->_id.detailed_id.id = id;
}

ID::ID(int grp_id, int id)
{
	this->_id.detailed_id.grp_id = grp_id;
	this->_id.detailed_id.id = id;
}

ID::ID(ID &a)
{
	this->_id.packed_id = a._id.packed_id;
}

ID::ID(const ID &a)
{
	this->_id.packed_id = a._id.packed_id;
}

ID::~ID()
{
}

void ID::setID(ID &id)
{
	this->_id.packed_id = id._id.packed_id;
}

string ID::getInfo() const
{
	stringstream changer;
	changer << getGid() << "_" << getId();
	string info;
	changer << info;
	return info; 
}

bool ID::operator==(const ID &id1)const{  
	return (this->_id.packed_id == id1._id.packed_id);  
}    

bool ID::operator<(const ID &id1)const{  
	if (this->_id.detailed_id.grp_id < id1._id.detailed_id.grp_id) {
		return true;
	}

	if (this->_id.detailed_id.id < id1._id.detailed_id.id) {
		return true;
	}

	return false;
}    
