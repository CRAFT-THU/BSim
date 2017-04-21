/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef NEURONBASE_H
#define NEURONBASE_H

#include "../base/Base.h"

class SynapseBase;

class NeuronBase : public Base {
public:
	NeuronBase(int node = 0);
	virtual ~NeuronBase() = 0;

	virtual void monitorOn();

	virtual bool isFired() final;
	virtual int getFireCount() final;

	virtual int fire() final;
	virtual int recv(real I) = 0;

	virtual  void setStartExec(int idx) final;
	virtual  void setStartInhi(int idx) final;
	virtual  void setEnd(int idx) final;

	virtual SynapseBase *addSynapse(SynapseBase *addSynapse) final;
	virtual SynapseBase *createSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *dst);

	const vector<SynapseBase*> & getSynapses() const;
protected:
	vector<SynapseBase*> pSynapses;
	bool fired;
	bool monitored;
	int fireCount;
	int _start_E;
	int _start_I;
	int _end;
};

#endif /* NEURONBASE_H */

