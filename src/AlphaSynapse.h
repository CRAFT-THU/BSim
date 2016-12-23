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
	Type getType();

	int init(real dt);
	virtual int recv();
	virtual size_t getSize();

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual int getData(void *data);
	virtual void setDst(NeuronBase *p);
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) = 0;

	AlphaSynapse(ID id, real weight, real delay, real tau_syn);
	~AlphaSynapse();

	const static Type type;
protected:
	real weight;
	real delay;
	int delay_steps;
	real C1;
	real C2;
	real _C1;
	real _C2;
	real tau_syn;
	real I_syn;
	real I_tmp;
	//real _dt;
	//bool monitored;
	list<int> delay_queue;
	NeuronBase *pDest;
};

#endif /* ALPHASYNAPSE_H */

