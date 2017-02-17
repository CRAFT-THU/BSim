/* This program is writen by qp09.
 * usually just for fun.
 * Wed October 28 2015
 */

#include <sstream>

#include "ID.h"

using std::stringstream;

ID::ID()
{
	this->id.packed_id = 0;
}

ID::ID(int id)
{
	this->id.detailed_id.machine_id = -1;
	this->id.detailed_id.type_id= -1;
	this->id.detailed_id.grp_id = -1;
	this->id.detailed_id.id = id;
}

ID::ID(int grp_id, int id)
{
	this->id.detailed_id.machine_id = -1;
	this->id.detailed_id.type_id= (grp_id >> 16) & 0xF;
	this->id.detailed_id.grp_id = grp_id & 0xFFFF;
	this->id.detailed_id.id = id;
}

ID::ID(short grp_id, int id)
{
	this->id.detailed_id.machine_id = -1;
	this->id.detailed_id.type_id= -1;
	this->id.detailed_id.grp_id = grp_id;
	this->id.detailed_id.id = id;
}

ID::ID(char type_id, short grp_id, int id)
{
	this->id.detailed_id.machine_id = -1;
	this->id.detailed_id.type_id = type_id;
	this->id.detailed_id.grp_id = grp_id;
	this->id.detailed_id.id = id;
}

ID::ID(char machine_id, char type_id, short grp_id, int id)
{
	this->id.detailed_id.type_id= type_id;
	this->id.detailed_id.machine_id = machine_id;
	this->id.detailed_id.grp_id = grp_id;
	this->id.detailed_id.id = id;
}

ID::ID(ID &a)
{
	this->id.packed_id = a.id.packed_id;
}

ID::ID(const ID &a)
{
	this->id.packed_id = a.id.packed_id;
}

ID::~ID()
{
}

void ID::advance()
{
	id.packed_id++;
}

void ID::setID(ID &id)
{
	this->id.packed_id = id.id.packed_id;
}

void ID::setNode(char node)
{
	this->id.detailed_id.machine_id = node;
}

string ID::getInfo() const
{
	stringstream changer;
	int grp_id = getGID();
	changer << grp_id << "_" << id.detailed_id.id;
	string ret;
	changer << ret;
	return ret; 
}

bool ID::operator==(const ID &id1)const{  
	return (this->id.packed_id == id1.id.packed_id);  
}    

bool ID::operator<(const ID &id1)const{  
	return (this->id.packed_id < id1.id.packed_id);  
}    
