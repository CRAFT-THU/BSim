
#include <assert.h>
#include <math.h>

#include "../../third_party/json/json.h"

#include "StaticSynapse.h"
#include "StaticData.h"

const Type StaticSynapse::type = Static;

StaticSynapse::StaticSynapse(real weight, real delay, real tau_syn)
	: Synapse(0, weight, delay), _tau_syn(tau_syn)
{
	this->monitored = false;
}

StaticSynapse::StaticSynapse(const StaticSynapse &synapse) : Synapse()
{
	this->_weight = synapse._weight;
	this->_delay = synapse._delay;
	this->_tau_syn = synapse._tau_syn;
	this->monitored = false;
}

StaticSynapse::~StaticSynapse()
{
	// delay_queue.clear();
}

// int StaticSynapse::recv()
// {
// 	delay_queue.push_back(_delay_steps);
// 
// 	return 0;
// }

Type StaticSynapse::getType() const
{
	return type;
}

// int StaticSynapse::reset(SimInfo &info)
// {
// 	real dt = info.dt;
// 	_delay_steps = static_cast<int>(round(_delay/dt));
// 	assert(fabs(_tau_syn) > ZERO || fabs(_delay-dt*_delay_steps) < ZERO);
// 	if (fabs(_tau_syn) > ZERO && fabs(_delay-dt*_delay_steps) > ZERO) {
// 		real _C1 = expf(-(_delay-dt*_delay_steps)/_tau_syn);
// 		this->_weight = this->_weight * _C1;
// 	}
// 
// 	return 0;
// }

// int StaticSynapse::update(SimInfo &info)
// {
// 	list<int>::iterator iter;
// 
// 	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
// 		this->_p_dst->recv(_weight);
// 		delay_queue.pop_front();
// 	}
// 
// 	for (iter = delay_queue.begin(); iter != delay_queue.end(); iter++) {
// 		*iter = *iter - 1;
// 	}
// 
// 	return 0;
// }

// void StaticSynapse::monitor(SimInfo &info)
// {
// }

// size_t StaticSynapse::getSize()
// {
// 	return sizeof(GStaticSynapses);
// }
// 
// int StaticSynapse::getData(void *data)
// {
// 	Json::Value *p = (Json::Value *)data;
// 	(*p)["id"] = getID();
// 	(*p)["weight"] = _weight;
// 
// 	return 0;
// }

int StaticSynapse::hardCopy(void * data, int idx, int base, SimInfo &info)
{
	StaticData *p = (StaticData *) data;

	real dt = info.dt;
	int delay_steps = static_cast<int>(round(_delay/dt));
	real weight = this->_weight;
	assert(fabs(_tau_syn) > ZERO);
	if (fabs(_tau_syn) > ZERO) {
		real c1 = exp(-(_delay-dt*delay_steps)/_tau_syn);
		weight = weight * c1;
	}
	setID(idx+base);

	p->pWeight[idx] = weight;
	//p->p_src[idx] = this->getSrc()->getID();
	p->pDst[idx] = this->getDst()->getID();

	return 1;
}

