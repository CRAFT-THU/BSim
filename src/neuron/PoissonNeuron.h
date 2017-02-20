/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef POISSONNEURON_H
#define POISSONNEURON_H

#include <stdio.h>
#include <vector>
#include <random>

#include "../base/NeuronBase.h"

using std::vector;
using std::default_random_engine;
using std::poisson_distribution;

class PoissonNeuron : public NeuronBase {
public:
	PoissonNeuron(ID id, real rate, real refract, real startTime = 0, real endTime = 1000);
	~PoissonNeuron();

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

	int poisson(int input);

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

#endif /* POISSONNEURON_H */

