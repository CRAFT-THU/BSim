/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef POSSIONNEURON_H
#define POSSIONNEURON_H

#include <stdio.h>
#include <vector>

#include "NeuronBase.h"

using std::vector;

class PossionNeuron : public NeuronBase {
public:
	PossionNeuron(ID id, double rate, double refract, double startTime = 0);
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
	virtual int hardCopy(void *data, int idx);
	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest);

	int possion(int input);

	const static Type type;
protected:
	vector<SynapseBase*> pSynapses;
	double m_rate;
	double m_refract;
	double m_startTime;
	double m_fireTime;
	double _dt;
	bool fired;
	bool monitored;
	int m_startCycle;
	int m_fireCycle;
	FILE *file;
};

#endif /* POSSIONNEURON_H */

