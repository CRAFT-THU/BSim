/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef BASE_H
#define BASE_H

#include "IDPool.h"
#include "SimInfo.h"

class Base {
public:
	virtual ID getID() = 0;
	virtual Type getType() = 0;

	virtual void monitorOn() = 0;
	virtual int reset(SimInfo &info) = 0;
	virtual int update(SimInfo &info) = 0;
	virtual void monitor(SimInfo &info) = 0;

	virtual size_t getSize() = 0;
	virtual int getData(void *data) = 0;
	virtual int hardCopy(void *data, int idx) = 0;
};

#endif /* BASE_H */

