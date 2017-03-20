/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef CONSTANTNEURON_H
#define CONSTANTNEURON_H

#include <stdio.h>
#include <vector>

#include "../base/NeuronBase.h"

using std::vector;

class ConstantNeuron: public NeuronBase {
public:
	ConstantNeuron(ID id, real fire_rate/*, real tau_syn_E = 1e-3, real tau_syn_I = 1e-3*/);
	ConstantNeuron(const ConstantNeuron &templ, ID id);
	~ConstantNeuron();

	virtual Type getType();

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id);

	int addFireTime(int cycle);

	const static Type type;
protected:
	int fire_count;
	real fire_rate;
	//real tau_syn_E;
	//real tau_syn_I;
	//real _dt;
	FILE *file;
};

#endif /* CONSTANTNEURON_H */
