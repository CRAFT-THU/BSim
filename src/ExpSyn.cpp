/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "ExpSyn.h"
#include <math.h>

ExpSyn::ExpSyn(real weight, real delay = 0, real tau_syn = 0)
{
	this->weight = weight;
	this->delay = delay;
	this->tau_syn = tau_syn;
	id = sid.getID();
	file = NULL;
}

ExpSyn::~ExpSyn()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

	delay_step.clear();
}

void ExpSyn::setDst(NeuronBase *p) {
	pDest = p;
}

int ExpSyn::reset(real dt) {
	I_syn = 0;
	init(dt);

	return 0;
}

int ExpSyn::init(real dt) {
	_dt = dt;
	if (tau_syn > 0) {
		C1 = expf(-dt/tau_syn);
		_C1 = expf(-delay/tau_syn);
	} else {
		printf("Wrong tau!\n");
		return -1;
	}

	return 0;
}

int ExpSyn::update()
{
	I_syn = C1 * I_syn;
	
	list<int>::iterator iter;
	for (iter = delay_step.begin(); iter != delay_step.end(); iter++) {
		*iter = *iter - 1;
	}

	if (!delay_step.empty() && delay_step.front() <= 0) {
		I_syn += weight/_C1;
		pDest->recv(I_syn);
		delay_step.pop_front();
	}
	return 0;
}

int ExpSyn::recv()
{
	delay_step.push_back((int) delay/_dt + 1);

	return 0;
}

void ExpSyn::monitor()
{
	if (file == NULL) {
		char filename[128];
		sprintf(filename, "Neuron_%d.log", id);
		file = fopen(filename, "w+");
	}
	fprintf(file, "%f", this->I_syn); 

	return;
}

size_t ExpSyn::getSize()
{
	return sizeof(ExpSyn);
}

size_t ExpSyn::hardCopy(unsigned char *data)
{
	memcpy(data, this, sizeof(ExpSyn));
	return sizeof(ExpSyn);
}
