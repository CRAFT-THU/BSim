/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef INPUTNEURON_H
#define INPUTNEURON_H

#include <stdio.h>
#include <vector>
#include <deque>

#include "NeuronBase.h"

using std::vector;
using std::deque;

class InputNeuron: public NeuronBase {
public:
	InputNeuron(ID id);
	~InputNeuron();

	virtual ID getID();

	virtual int fire();
	virtual bool isFired();
	virtual int recv(real I);

	virtual void monitorOn();
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual unsigned int hardCopy(void *data, unsigned int idx);
	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);

	int addFireTime(unsigned int cycle);
private:
	vector<SynapseBase*> pSynapses;
	deque<unsigned int> fireTime;
	double _dt;
	bool fired;
	bool monitored;
	FILE *file;
	ID m_id;
};

#endif /* INPUTNEURON_H */

