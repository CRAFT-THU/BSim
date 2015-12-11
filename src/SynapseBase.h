/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "constant.h"
#include "NeuronBase.h"

class SynapseBase {
public:
	virtual ~SynapseBase() = 0;

	virtual int recv() = 0;
	virtual ID getID() = 0;
	virtual int update() = 0;
	virtual void monitor() = 0;
	virtual size_t getSize() = 0;
	virtual int reset(real dt) = 0;
	virtual void setDst(NeuronBase *p) = 0;
	virtual size_t hardCopy(void *data) = 0;
};

#endif /* SYNAPSEBASE_H */

