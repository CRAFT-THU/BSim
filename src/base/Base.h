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
	//Base() = delete;
	Base(ID id, int node=0);
	virtual ~Base();

	inline ID getID() {
		return _id;
	}

	inline size_t getIdx() {
		return _idx;
	}

	inline int getNode() {
		return _node;
	}

	inline void setID(ID id) {
		this->_id = id;
	}

	inline void setIdx(size_t idx) {
		this->_idx = idx;
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
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) = 0;

	
private:
	ID _id;
	size_t _idx;
	int _node;
};

#endif /* BASE_H */

