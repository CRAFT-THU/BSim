/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "constant.h"

class SynapseBase {
public:
	virtual int update() = 0;
	virtual int recv() = 0;
	virtual int reset(float dt) = 0;
protected:
	floatn weight;
	floatn delay;
};

#endif /* SYNAPSEBASE_H */

