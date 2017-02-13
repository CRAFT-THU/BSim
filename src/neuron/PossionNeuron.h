/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef POSSIONNEURON_H
#define POSSIONNEURON_H

#include <stdio.h>
#include <vector>
#include <random>

#include "NeuronBase.h"

using std::vector;
using std::default_random_engine;
using std::poisson_distribution;

class PossionNeuron : public NeuronBase {
public:
	PossionNeuron(ID id, real rate, real refract, real startTime = 0, real endTime = 1000);
	~PossionNeuron();

	virtual Type getType();

	virtual int fire();
	virtual bool isFired();

	virtual void monitorOn();
	virtual int update(SimInfo &info);
	virtual int reset(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual int recv(real I);

	virtual int getData(void*);

	virtual size_t getSize();
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id);
	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest);

	int possion(int input);

	const static Type type;
protected:
	vector<SynapseBase*> pSynapses;
	default_random_engine generator;
	poisson_distribution<int> distribution;
	real rate;
	real refract;
	real startTime;
	real endTime;
	//real fireTime;
	//real _dt;
	bool fired;
	bool monitored;
	int startCycle;
	int fireCycle;
	int endCycle;
	int refract_step;
	FILE *file;
};

#endif /* POSSIONNEURON_H */

