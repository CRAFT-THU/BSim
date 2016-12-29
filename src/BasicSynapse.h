/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef BASICSYNAPSE_H
#define BASICSYNAPSE_H

#include <list>
#include <stdio.h>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

class BasicSynapse : public SynapseBase {
public:
	Type getType();

	virtual int recv();
	virtual void setDst(NeuronBase *p);

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id);

	BasicSynapse(ID id, real weight, real delay, real tau_syn);
	~BasicSynapse(); 

	const static Type type;
protected:
	real weight;
	real delay;
	int delay_steps;

	list<int> delay_queue;
	NeuronBase *pDest;
	bool monitored;
	FILE* file;
};

#endif /* BASICSYNAPSE_H */

