/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef ARRAYNEURON_H
#define ARRAYNEURON_H

#include <stdio.h>
#include <vector>
#include <deque>

#include "../../base/NeuronBase.h"

using std::vector;
//using std::deque;

class ArrayNeuron: public NeuronBase {
public:
	ArrayNeuron();
	// The array must be ordered. from small to large.
	ArrayNeuron(int *array, int num);
	ArrayNeuron(const ArrayNeuron &templ);
	~ArrayNeuron();

	virtual Type getType();

	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base);

	int addFireTime(int cycle);

	vector<int> & getFireTimes();

	const static Type type;
protected:
	vector<int> fireTime;
	int idx;
	FILE *file;
};

#endif /* ARRAYNEURON_H */
