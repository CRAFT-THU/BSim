/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef BASE_H
#define BASE_H

#include <map>

#include "../utils/SimInfo.h"

using std::map;

class Base {
public:
	Base(int node=0) : _id(0), _node(node) {}
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
	// virtual int reset(SimInfo &info) {
	// 	printf("Reset is depracated, it is not required anymore.");
	// 	return 0;
	// }
	// virtual int update(SimInfo &info) {
	// 	printf("Update is depracated, it is not required anymore.");
	// 	return 0;
	// }
	// virtual void monitor(SimInfo &info) {
	// 	printf("monitor is depracated, it is not required anymore.");
	// }

	virtual size_t getSize() = 0;
	virtual int getData(void *data) = 0;
	virtual int hardCopy(void *data, int idx, int base, SimInfo &info) = 0;
	
private:
	ID _id;
	int _node;
};

#endif /* BASE_H */

