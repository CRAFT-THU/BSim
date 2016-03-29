/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef NENGOLOWPASS_H
#define NENGOLOWPASS_H

#include <list>
#include <stdio.h>
#include "NengoBase.h"

using std::list;

class NengoLowpass {
public:
	ID getID();
	int getSize();
	Type getType();

	int init(real dt);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);


	NengoLowpass(ID id, int src_size, int dst_size, real *weight, real tau_syn, real *fired, real *i_syn);
	virtual ~NengoLowpass(); 

	const static Type type;
protected:
	ID id;
	real *fired;
	real *weight;
	real *output;
	real *i_syn;
	real C1;
	real _C1;
	real tau_syn;
	int src_size;
	int dst_size;
	real _dt;
	FILE* file;
};

#endif /* NENGOLOWPASS_H */

