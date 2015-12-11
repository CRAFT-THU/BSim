/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef EXPSYNAPSE_H
#define EXPSYNAPSE_H

#include <list>
#include <stdio.h>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

struct GExpSynapse {
	SpikeType type;
	real weight;
	real delay;
	real C1;
	real _C1;
	real tau_syn;
	real I_syn;
	real _dt;
	ID id;
	ID src;
	ID dst;
};

class ExpSynapse : public SynapseBase {
public:
	ID getID();
	int init(real dt);
	virtual int recv();
	virtual int update();
	virtual void monitor();
	virtual size_t getSize();
	virtual int reset(real dt);
	virtual void setDst(NeuronBase *p);
	virtual size_t hardCopy(void *data);

	ExpSynapse(ID id, real weight, real delay, real tau_syn);
	~ExpSynapse(); 

protected:
	SpikeType type;
	real weight;
	real delay;
	real C1;
	real _C1;
	real tau_syn;
	real I_syn;
	real _dt;
	ID id;
	list<int> delay_step;
	NeuronBase *pDest;
	FILE* file;
};

#endif /* EXPSYNAPSE_H */

