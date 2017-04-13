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
	Base(ID id, int node=0) : _id(id), _node(node) {}
	virtual ~Base() {}

	inline ID getID() {
		return _id;
	}

	inline int getNode() {
		return _node;
	}

	inline void setID(ID id) {
		this->_id = id;
	}

	inline void setNode(int node) {
		this->_node = node;
	}

	virtual Type getType() = 0;
	virtual int reset(SimInfo &info) = 0;
	virtual int update(SimInfo &info) = 0;
	virtual void monitor(SimInfo &info) = 0;

	virtual size_t getSize() = 0;
	virtual int getData(void *data) = 0;
	virtual int hardCopy(void *data, int idx, int base) = 0;

	
private:
	ID _id;
	int _node;
};

#endif /* BASE_H */

