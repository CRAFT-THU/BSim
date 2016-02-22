/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef SYNAPSEBASE_H
#define SYNAPSEBASE_H

#include "NeuronBase.h"

class SynapseBase : public Base {
public:
	virtual ~SynapseBase() = 0;
	

	virtual int recv() = 0;
	virtual void monitorOn() = 0;
	virtual void setDst(NeuronBase *p) = 0;
};

#endif /* SYNAPSEBASE_H */

