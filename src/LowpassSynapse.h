/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef LOWPASSSYNAPSE_H
#define LOWPASSSYNAPSE_H

#include <list>
#include <stdio.h>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

class LowpassSynapse : public SynapseBase {
public:
	ID getID();

	int init(real dt);
	virtual int recv();
	virtual void setDst(NeuronBase *p);

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual unsigned int hardCopy(void *data, unsigned int idx);

	LowpassSynapse(ID id, real weight, real delay, real tau_syn);
	~LowpassSynapse(); 

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
	bool monitored;
	FILE* file;
};

#endif /* LOWPASSSYNAPSE_H */

