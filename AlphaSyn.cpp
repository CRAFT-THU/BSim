/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "AlphaSyn.h"

AlphaSyn::AlphaSyn(float weight = 1, float delay = 0, float tau_syn = 0)
{
	this.weight = weight;
	this.delay = delay;
	this.tau_syn = tau_syn;
}

int AlphaSyn::reset(float dt) {
	I_syn = 0;
	I_tmp = 0;
	init(dt);

	return 0;
}

int AlphaSyn::init(float dt) {
	if (tau_syn > 0) {
		C1 = exp(-dt/tau_syn);
		C2 = (dt/tau_syn) * exp(1.0f - delta/tau);
		_C1 = exp(-delay/tau_syn);
		_C2 = (delay/tau_syn) * exp(1.0f -delta/tau);
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

	return 0;
}

int AlphaSyn::recv()
{
	double I_t = _C1 * I_syn + _C2 * I_tmp;
	I_tmp += weight/_C1;
	I_syn = (I_t - _C2 * I_tmp)/_C1;

	return 0;
}
