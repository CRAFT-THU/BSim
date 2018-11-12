/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef FFTNEURON_H
#define FFTNEURON_H

#include <stdio.h>
#include <vector>

#include "../../base/NeuronBase.h"

using std::vector;

class FFTNeuron: public NeuronBase {
public:
	FFTNeuron();
	FFTNeuron(const FFTNeuron &templ);
	~FFTNeuron();

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
	int fire_count;
	FILE *file;
};

#endif /* FFTNEURON_H */
