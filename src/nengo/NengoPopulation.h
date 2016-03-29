/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef NENGOPOPULATION_H
#define NENGOPOPULATION_H

#include <stdio.h>

#include "../utils/json/json.h"
#include "NengoBase.h"

class NengoPopulation : public NengoBase {
public:
	NengoPopulation(Json::Value population);
	~NengoPopulation();

	ID getID();
	int getSize();
	Type getType();

	virtual int init(real dt);
	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);


	virtual real* getInput();
	virtual real* getOutput();

	const static Type type;
protected:
	ID id;
	real *v_init;
	real *v_min;
	real *v_reset;
	real *tau_m;
	real *tau_refrac;
	real *v_thresh;
	real *i_offset;
	real *encoder;
	real *i_syn;
	real *vm;
	real *C1;
	real *refrac_time;
	real *fire_array;
	real _dt;
	int size;
	int dim;
	FILE* file;
};

#endif /* NENGOPOPULATION_H */

