/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef MAXNEURON_H
#define MAXNEURON_H

#include <stdio.h>

#include "../base/NeuronBase.h"

class MaxNeuron : public NeuronBase {
public:
	MaxNeuron(ID id, int N);
	MaxNeuron(const MaxNeuron &neuron, ID id);
	MaxNeuron(const MaxNeuron &neuron);
	~MaxNeuron();

	virtual Type getType();

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id);

	const static Type type;
protected:
	int *_record;
	int _count;
	int _idxs;
	int _N;
};

#endif /* MAXNEURON_H */

