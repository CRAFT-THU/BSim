/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef NENGOBASE_H
#define NENGOBASE_H

#include "../utils/IDPool.h"
#include "../base/SimInfo.h"

class NengoBase {
public:
	virtual ~NengoBase();

	virtual ID getID() = 0;
	virtual int getSize() = 0;
	virtual Type getType() = 0;

	virtual int reset(SimInfo &info) = 0;
	virtual int update(SimInfo &info) = 0;

	virtual real* getOutput() = 0;
	virtual real* getInput() = 0;
};

#endif /* NENGOBASE_H */

