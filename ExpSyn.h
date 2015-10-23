/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef EXPSYN_H
#define EXPSYN_H

#include "SynapseBase.h"

class ExpSyn : SynapseBase {
public:
	int init(float dt);
	virtual int update();
	virtual int recv();
	virtual int reset(float dt);

	ExpSyn(float weight, float delay);
	~ExpSyn() {}

protected:
	float weight;
	float delay;
	float C1;
	float _C1;
	float tau_syn;
	float I_syn;
}

#endif /* EXPSYN_H */

