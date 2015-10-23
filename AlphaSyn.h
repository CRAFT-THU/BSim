/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef ALPHASYN_H
#define ALPHASYN_H

#include "SynapseBase.h"

class AlphaSyn : SynapseBase {
public:
	int init(float dt);
	virtual int update();
	virtual int recv();
	virtual int reset(float dt);

	AlphaSyn(float weight, float delay);
	~AlphaSyn() {}

protected:
	float weight;
	float delay;
	float C1;
	float C2;
	float _C1;
	float _C1;
	float tau_syn;
	float I_syn;
	float I_tmp;
}

#endif /* ALPHASYN_H */

