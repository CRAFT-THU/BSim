#ifndef STATICSYNAPSE_H
#define STATICSYNAPSE_H

#include <stdio.h>
#include <list>
#include "../base/SynapseBase.h"

using std::list;

class StaticSynapse : public SynapseBase {
public:
	StaticSynapse(ID id, real weight, real delay, real tau_syn);
	StaticSynapse(const StaticSynapse &synapse, ID id);
	~StaticSynapse();

	virtual int recv()  override;
	virtual void setDst(NeuronBase *p)  override;

	virtual Type getType() override;

	virtual int reset(SimInfo &info) override;
	virtual int update(SimInfo &info) override;
	virtual void monitor(SimInfo &info) override;

	virtual size_t getSize() override;
	virtual int getData(void *data) override;
	virtual int hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id) override;

protected:
	const static Type type;
	real _weight;
	real _delay;
	real _tau_syn;
	list<int> delay_queue;
	NeuronBase *pDest;
};

#endif /* STATICSYNAPSE_H */
