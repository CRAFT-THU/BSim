/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "constant.h"
#include "NeuronBase.h"

extern ID sid;

class SynapseBase {
public:
	virtual ~SynapseBase();
	virtual int update() = 0;
	virtual int recv() = 0;
	virtual int reset(real dt) = 0;
	virtual void setDst(NeuronBase *p) = 0;
	virtual void monitor() = 0;
protected:
	real weight;
	real delay;
	SynType type;
};

#endif /* SYNAPSEBASE_H */

