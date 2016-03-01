/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include <math.h>
#include <cstdio>

#include "utils/json/json.h"
#include "AlphaSynapse.h"
#include "GAlphaSynapses.h"

AlphaSynapse::AlphaSynapse(ID id, real weight, real delay = 0.0, real tau_syn = 0.0)
{
	this->weight = weight;
	this->delay = delay;
	this->tau_syn = tau_syn;
	this->id = id;
	this->monitored = false;
}

AlphaSynapse::~AlphaSynapse()
{
	delay_step.clear();
}

void AlphaSynapse::setDst(NeuronBase *p) {
	pDest = p;
}

int AlphaSynapse::reset(SimInfo &info) {
	I_syn = 0;
	I_tmp = 0;
	init(info.dt);

	return 0;
}

int AlphaSynapse::init(real dt) {
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

int AlphaSynapse::update(SimInfo &info)
{
	I_syn = C1 * I_syn + C2 * I_tmp;
	I_tmp *= C1;

	list<int>::iterator iter;

	if (!delay_step.empty() && delay_step.front() <= 0) {
		double I_t = _C2 * I_syn + _C2 * I_tmp;
		I_tmp += weight/_C1;
		I_syn = (I_t - _C2 * I_tmp)/_C1;
		pDest->recv(I_syn);
		delay_step.pop_front();
	}

	for (iter = delay_step.begin(); iter != delay_step.end(); iter++) {
		*iter = *iter - 1;
	}

	return 0;
}

int AlphaSynapse::recv()
{
	delay_step.push_back((int)(delay/_dt));

	return 0;
}

void AlphaSynapse::monitor(SimInfo &info)
{

	return;
}

size_t AlphaSynapse::getSize()
{
	return sizeof(GAlphaSynapses);
}

ID AlphaSynapse::getID()
{
	return id;
}

int AlphaSynapse::getData(void *data)
{
	Json::Value *p = (Json::Value*)data;
	(*p)["weight"] = weight;
	(*p)["delay"] = delay;

	return 0;
}

int AlphaSynapse::hardCopy(void *data, int idx)
{
	GAlphaSynapses *p = (GAlphaSynapses*)data;
	p->pType[idx] = type;
	p->p_weight[idx] = weight;
	p->p_delay[idx] = delay;
	p->p_C1[idx]= C1;
	p->p_C2[idx] = C2;
	p->p__C1[idx] = _C1;
	p->p__C2[idx] = _C2;
	p->p_tau_syn[idx] = tau_syn;
	p->p_I_syn[idx] = I_syn;
	p->p_I_tmp[idx] = I_tmp;
	p->p__dt[idx] = _dt;
	p->pID[idx] = id;
	return 1;
}
