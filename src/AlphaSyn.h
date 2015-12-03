/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef ALPHASYN_H
#define ALPHASYN_H

#include <list>
#include "SynapseBase.h"
#include "NeuronBase.h"

using std::list;

class AlphaSyn : SynapseBase {
public:
	int init(real dt);
	virtual void setDst(NeuronBase *p);
	virtual int update();
	virtual int recv();
	virtual int reset(real dt);
	virtual void monitor();
	virtual size_t getSize();
	virtual size_t hardCopy(unsigned char *data);

	AlphaSyn(real weight, real delay, real tau_syn);
	~AlphaSyn();

protected:
	SynType type;
	real weight;
	real delay;
	real C1;
	real C2;
	real _C1;
	real _C2;
	real tau_syn;
	real I_syn;
	real I_tmp;
	real _dt;
	list<int> delay_step;
	NeuronBase *pDest;
	int id;
};

#endif /* ALPHASYN_H */

