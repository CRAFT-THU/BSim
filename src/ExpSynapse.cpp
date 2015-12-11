/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "ExpSynapse.h"
#include <math.h>

ExpSynapse::ExpSynapse(ID id, real weight, real delay = 0, real tau_syn = 0)
{
	this->weight = weight;
	this->delay = delay;
	this->tau_syn = tau_syn;
	this->id = id;
	file = NULL;
}

ExpSynapse::~ExpSynapse()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

	delay_step.clear();
}

void ExpSynapse::setDst(NeuronBase *p) {
	pDest = p;
}

int ExpSynapse::reset(real dt) {
	I_syn = 0;
	init(dt);

	return 0;
}

int ExpSynapse::init(real dt) {
	_dt = dt;
	if (tau_syn > 0) {
		C1 = expf(-dt/tau_syn);
		_C1 = expf(-(delay-dt*(int)(delay/dt))/tau_syn);
	} else {
		printf("Wrong tau!\n");
		return -1;
	}

	return 0;
}

int ExpSynapse::update()
{
	I_syn = C1 * I_syn;
	
	list<int>::iterator iter;
	for (iter = delay_step.begin(); iter != delay_step.end(); iter++) {
		*iter = *iter - 1;
	}

	while (!delay_step.empty() && (delay_step.front() <= 0)) {
		I_syn += weight/_C1;
		pDest->recv(I_syn);
		delay_step.pop_front();
	}
	return 0;
}

int ExpSynapse::recv()
{
	delay_step.push_back((int) delay/_dt + 1);

	return 0;
}

ID ExpSynapse::getID()
{
	return id;
}

void ExpSynapse::monitor()
{
	if (file == NULL) {
		char filename[128];
		sprintf(filename, "Neuron_%d.log", id.id);
		file = fopen(filename, "w+");
	}
	fprintf(file, "%f\n", this->I_syn); 

	return;
}

size_t ExpSynapse::getSize()
{
	return sizeof(GExpSynapse);
}

size_t ExpSynapse::hardCopy(void *data)
{
	GExpSynapse *p = (GExpSynapse *)data;
	p->type = type;
	p->weight = weight;
	p->delay = delay;
	p->C1 = C1;
	p->_C1 = _C1;
	p->tau_syn = tau_syn;
	p->I_syn = I_syn;
	p->_dt = _dt;
	p->id = id;
	return sizeof(GExpSynapse);
}
