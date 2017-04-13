/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef EXPSYNAPSE_H
#define EXPSYNAPSE_H

#include <list>
#include <stdio.h>
#include "../base/SynapseBase.h"

using std::list;

class ExpSynapse : public SynapseBase {
public:
	virtual Type getType() override;

	int init(real dt);
	virtual int recv() override;
	//virtual void setDst(NeuronBase *p);

	virtual void monitorOn() override;
	virtual int reset(SimInfo &info) override;
	virtual int update(SimInfo &info) override;
	virtual void monitor(SimInfo &info) override;

	virtual size_t getSize() override;
	virtual int getData(void *data) override;
	virtual int hardCopy(void *data, int idx, int base) override;

	ExpSynapse(ID id, real weight, real delay, real tau_syn);
	~ExpSynapse(); 

	const static Type type;
protected:
	//real weight;
	//int delay_setps;
	int active_steps;
	int step_to_zero;
	real delay;
	real C1;
	real _C1;
	real tau_syn;
	real I_syn;
	//real _dt;
	list<int> delay_queue;
	//NeuronBase *pDest;
	bool monitored;
	FILE* file;
};

#endif /* EXPSYNAPSE_H */

