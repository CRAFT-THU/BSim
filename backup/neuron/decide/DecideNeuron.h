/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef DECIDENEURON_H
#define DECIDENEURON_H

#include <stdio.h>
#include <vector>

#include "../../base/NeuronBase.h"

using std::vector;

class DecideNeuron: public NeuronBase {
public:
	DecideNeuron();
	DecideNeuron(const DecideNeuron &templ);
	~DecideNeuron();

	virtual Type getType();

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base);

	const static Type type;
protected:
	real fire_rate;
	real tmp_rate;
	FILE *file;
};

#endif /* DECIDENEURON_H */
