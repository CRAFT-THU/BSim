/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef NENGOPROBE_H
#define NENGOPROBE_H

#include <stdio.h>

#include "NengoBase.h"

class NengoProbe : public NengoBase {
public:
	NengoProbe(ID id, int size, real *weight, real *fire_array);
	~NengoProbe();

	virtual ID getID();
	virtual int getSize();
	virtual Type getType();

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);

	virtual real* getOutput();
	virtual real* getInput();

	int init(real dt);

	const static Type type;
protected:
	ID id;
	real _dt;
	real vm;
	real *probes;
	real *weights;
	int size;
	FILE* file;
};

#endif /* NENGOPROBE_H */

