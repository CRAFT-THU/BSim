/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef BASE_H
#define BASE_H

#include <map>

#include "SimInfo.h"

using std::map;

class Base {
public:
	Base();
	virtual ~Base();

	virtual ID getID();
	virtual void setID(ID id);
	virtual Type getType() = 0;

	virtual void monitorOn() = 0;
	virtual int reset(SimInfo &info) = 0;
	virtual int update(SimInfo &info) = 0;
	virtual void monitor(SimInfo &info) = 0;

	virtual size_t getSize() = 0;
	virtual int getData(void *data) = 0;
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) = 0;
	
protected:
	ID id;
};

#endif /* BASE_H */

