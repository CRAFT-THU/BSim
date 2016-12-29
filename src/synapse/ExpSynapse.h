/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef EXPSYNAPSE_H
#define EXPSYNAPSE_H

#include <list>
#include <stdio.h>
#include "SynapseBase.h"

using std::list;

class ExpSynapse : public SynapseBase {
public:
	Type getType();

	int init(real dt);
	virtual int recv();
	virtual void setDst(NeuronBase *p);

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id);

	ExpSynapse(ID id, real weight, real delay, real tau_syn);
	~ExpSynapse(); 

	const static Type type;
protected:
	real weight;
	real delay;
	int delay_steps;
	real C1;
	real _C1;
	real tau_syn;
	real I_syn;
	//real _dt;
	list<int> delay_queue;
	NeuronBase *pDest;
	bool monitored;
	FILE* file;
};

#endif /* EXPSYNAPSE_H */

