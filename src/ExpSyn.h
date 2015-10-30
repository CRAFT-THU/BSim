/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef EXPSYN_H
#define EXPSYN_H

#include <list>
#include <stdio.h>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

class ExpSyn : public SynapseBase {
public:
	int init(real dt);
	virtual void setDst(NeuronBase *p);
	virtual int update();
	virtual int recv();
	virtual int reset(real dt);
	virtual void monitor();

	ExpSyn(real weight, real delay, real tau_syn);
	~ExpSyn(); 

protected:
	real weight;
	real delay;
	real C1;
	real _C1;
	real tau_syn;
	real I_syn;
	real _dt;
	list<int> delay_step;
	NeuronBase *pDest;
	int id;
	FILE* file;
};

#endif /* EXPSYN_H */

