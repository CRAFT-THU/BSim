/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "AlphaSyn.h"
#include <math.h>
#include <cstdio>

AlphaSyn::AlphaSyn(real weight, real delay = 0.0, real tau_syn = 0.0)
{
	this->weight = weight;
	this->delay = delay;
	this->tau_syn = tau_syn;
	id = sid.getID();
}

AlphaSyn::~AlphaSyn()
{
	delay_step.clear();
}

void AlphaSyn::setDst(NeuronBase *p) {
	pDest = p;
}

int AlphaSyn::reset(real dt) {
	I_syn = 0;
	I_tmp = 0;
	init(dt);

	return 0;
}

int AlphaSyn::init(real dt) {
	_dt = dt;
	if (tau_syn > 0) {
		C1 = expf(-dt/tau_syn);
		C2 = (dt/tau_syn) * expf(1.0f - dt/tau_syn);
		_C1 = expf(-delay/tau_syn);
		_C2 = (delay/tau_syn) * exp(1.0f - dt/tau_syn);
	} else {
		printf("Wrong tau!\n");
		return -1;
	}

	return 0;
}

int AlphaSyn::update()
{
	I_syn = C1 * I_syn + C2 * I_tmp;
	I_tmp *= C1;

	list<int>::iterator iter;
	for (iter = delay_step.begin(); iter != delay_step.end(); iter++) {
		*iter = *iter - 1;
	}

	if (!delay_step.empty() && delay_step.front() <= 0) {
		double I_t = _C2 * I_syn + _C2 * I_tmp;
		I_tmp += weight/_C1;
		I_syn = (I_t - _C2 * I_tmp)/_C1;
		pDest->recv(I_syn);
		delay_step.pop_front();
	}

	return 0;
}

int AlphaSyn::recv()
{
	delay_step.push_back((int) delay/_dt + 1);

	return 0;
}

void AlphaSyn::monitor()
{

	return;
}
