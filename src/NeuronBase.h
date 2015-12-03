/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef NEURONBASE_H
#define NEURONBASE_H

#include "constant.h"
#include "utils/ID.h"

extern ID nid;

class SynapseBase;

class NeuronBase {
public:
	virtual ~NeuronBase();

	virtual int reset(real dt) = 0;
	virtual int recv(real I) = 0;
	virtual int update() = 0;
	virtual bool is_fired() = 0;
	virtual void monitor() = 0;
	virtual SynapseBase* addSyn(real weight, real delay, SynType type, NeuronBase *pDest) = 0;
	virtual int fire() = 0;
	virtual size_t getSize() = 0;
	virtual size_t hardCopy(unsigned char *data) = 0;
};

#endif /* NEURONBASE_H */

