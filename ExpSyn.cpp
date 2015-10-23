/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "ExpSyn.h"

ExpSyn::ExpSyn(float weight = 1, float delay = 0, float tau_syn = 0)
{
	this.weight = weight;
	this.delay = delay;
	this.tau_syn = tau_syn;
}

int ExpSyn::reset(float dt) {
	I_syn = 0;
	init(dt);

	return 0;
}

int ExpSyn::init(float dt) {
	if (tau_syn > 0) {
		C1 = exp(-dt/tau_syn);
		_C1 = exp(-delay/tau_syn);
	} else {
		printf("Wrong tau!\n");
		return -1;
	}

	return 0;
}

int ExpSyn::update()
{
	I_syn = C1 * I_syn;

	return 0;
}

int ExpSyn::recv()
{
	I_syn += weight/_C1;

	return 0;
}
