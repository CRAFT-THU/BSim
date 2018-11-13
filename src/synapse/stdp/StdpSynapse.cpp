
#include <assert.h>
#include <math.h>

#include "../../third_party/json/json.h"

#include "StdpSynapse.h"
#include "GStdpSynapses.h"

const Type StdpSynapse::type = Stdp;

StdpSynapse::StdpSynapse(real weight, real delay, real tau_syn)
	: SynapseBase(0, weight), _delay(delay), _tau_syn(tau_syn)
{
	this->monitored = false;
}

StdpSynapse::StdpSynapse(const StdpSynapse &synapse) : SynapseBase()
{
	this->_weight = synapse._weight;
	this->_delay = synapse._delay;
	this->_tau_syn = synapse._tau_syn;
	this->_apre = synapse._apre;
	this->_delay = synapse._delay;
	this->_tau_syn = synapse._tau_syn;
	this->monitored = false;
}

StdpSynapse::~StdpSynapse()
{
	delay_queue.clear();
}

int StdpSynapse::recv()
{
	delay_queue.push_back(_delay_steps);

	return 0;
}

Type StdpSynapse::getType()
{
	return type;
}

int StdpSynapse::reset(SimInfo &info)
{
	real dt = info.dt;
	_delay_steps = static_cast<int>(round(_delay/dt));
	assert(fabs(_tau_syn) > ZERO || fabs(_delay-dt*_delay_steps) < ZERO);
	if (fabs(_tau_syn) > ZERO && fabs(_delay-dt*_delay_steps) > ZERO) {
		real _C1 = expf(-(_delay-dt*_delay_steps)/_tau_syn);
		this->_weight = this->_weight * _C1;
	}

	return 0;
}

int StdpSynapse::update(SimInfo &info)
{
	list<int>::iterator iter;

	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
		this->_p_dst->recv(_weight);
		delay_queue.pop_front();
	}

	for (iter = delay_queue.begin(); iter != delay_queue.end(); iter++) {
		*iter = *iter - 1;
	}

	return 0;
}

void StdpSynapse::monitor(SimInfo &info)
{
}

size_t StdpSynapse::getSize()
{
	return sizeof(GStdpSynapses);
}

int StdpSynapse::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID();
	(*p)["weight"] = _weight;

	return 0;
}

int StdpSynapse::hardCopy(void * data, int idx, int base)
{
	GStdpSynapses *p = (GStdpSynapses *) data;
	setID(idx+base);
	p->p_weight[idx] = _weight;
	//p->p_delay[idx] = this->getDelay();
	//p->p_src[idx] = this->getSrc()->getID();
	p->p_dst[idx] = this->getDst()->getID();

	return 1;
}

