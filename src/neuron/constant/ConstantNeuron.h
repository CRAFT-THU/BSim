/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef CONSTANTNEURON_H
#define CONSTANTNEURON_H

#include <stdio.h>
#include <vector>

#include "../../base/NeuronBase.h"

using std::vector;

class ConstantNeuron: public NeuronBase {
public:
	ConstantNeuron(real fire_rate, real start_time = 0.0, real end_time = 1.0e9);
	ConstantNeuron(const ConstantNeuron &templ);
	~ConstantNeuron();

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
	void setStart(real time);
	void setEnd(real time);

	const static Type type;
protected:
	real fire_rate;
	int start_cycle;
	int end_cycle;


	real start_time;
	real end_time;

	FILE *file;
};

#endif /* CONSTANTNEURON_H */
