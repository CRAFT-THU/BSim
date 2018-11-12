/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef MEMNEURON_H
#define MEMNEURON_H

#include <stdio.h>
#include <vector>

#include "../../base/NeuronBase.h"

using std::vector;

class MemNeuron: public NeuronBase {
public:
	MemNeuron(real fire_rate/*, real tau_syn_E = 1e-3, real tau_syn_I = 1e-3*/);
	MemNeuron(const MemNeuron &templ);
	~MemNeuron();

	virtual Type getType();

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base);

	int addFireTime(int cycle);
	void setRate(real rate);

	const static Type type;
protected:
	//int fire_count;
	real fire_rate;
	//real tau_syn_E;
	//real tau_syn_I;
	//real _dt;
	FILE *file;
};

#endif /* MEMNEURON_H */
