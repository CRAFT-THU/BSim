/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef MAXNEURON_H
#define MAXNEURON_H

#include <stdio.h>

#include "NeuronBase.h"

class MaxNeuron : public NeuronBase {
public:
	MaxNeuron(ID id, int cycle);
	MaxNeuron(const MaxNeuron &neuron, ID id);
	~MaxNeuron();

	Type getType();

	virtual real get_vm();
	virtual int init(real dt);

	virtual int fire();
	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void * data, int idx);

	const static Type type;
protected:
	vector<int> input_count;
	int max_input;
	int fired_count;
	int cycle;
	real _dt;
	FILE* file;
};

#endif /* MAXNEURON_H */

