/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef ALPHASYNAPSE_H
#define ALPHASYNAPSE_H

#include <list>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

class AlphaSynapse : SynapseBase {
public:
	ID getID();
	int init(real dt);
	virtual int recv();
	virtual size_t getSize();

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual int getData(void *data);
	virtual void setDst(NeuronBase *p);
	virtual int hardCopy(void *data, int idx);

	AlphaSynapse(ID id, real weight, real delay, real tau_syn);
	~AlphaSynapse();

protected:
	SpikeType type;
	real weight;
	real delay;
	real C1;
	real C2;
	real _C1;
	real _C2;
	real tau_syn;
	real I_syn;
	real I_tmp;
	real _dt;
	ID id;
	bool monitored;
	list<int> delay_step;
	NeuronBase *pDest;
};

#endif /* ALPHASYNAPSE_H */
