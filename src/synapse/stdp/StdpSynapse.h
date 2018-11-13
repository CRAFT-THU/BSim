#ifndef STATICSYNAPSE_H
#define STATICSYNAPSE_H

#include <stdio.h>
#include <list>
#include "../../base/SynapseBase.h"

using std::list;

class StdpSynapse : public SynapseBase {
public:
	StdpSynapse(real weight, real delay, real tau_syn, real apre, real d_apre, real apost, real d_apost, real tau_pre, tau_post);
	StdpSynapse(const StdpSynapse &synapse);
	~StdpSynapse();

	virtual int recv()  override;
	//virtual void setDst(NeuronBase *p)  override;

	virtual Type getType() override;

	virtual int reset(SimInfo &info) override;
	virtual int update(SimInfo &info) override;
	virtual void monitor(SimInfo &info) override;

	virtual size_t getSize() override;
	virtual int getData(void *data) override;
	virtual int hardCopy(void * data, int idx, int base) override;

protected:
	const static Type type;
	//real _weight;
	real _delay;
	real _tau_syn;
	real _apre;
	real _d_apre;
	real _apost;
	real _d_apost;
	real _tau_pre;
	real _tau_post;
	list<int> delay_queue;
	//NeuronBase *pDest;
};

#endif /* STATICSYNAPSE_H */
