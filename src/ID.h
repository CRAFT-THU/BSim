/* This header file is writen by qp09
 * usually just for fun
 * Wed October 28 2015
 */
#ifndef ID_H
#define ID_H

#include "constant.h"

struct ID {
public:
	ID();
	ID(int _groupId, int _id);
	ID(int _id);
	ID(const ID &id);
	~ID();
	bool operator==(const ID &id1)const; 
	bool operator<(const ID &id1)const; 

//private:
	int groupId;
	int id;
};

#endif /* ID_H */

