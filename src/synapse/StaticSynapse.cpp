
#include <math.h>

#include "../third_party/json/json.h"
#include "StaticSynapse.h"
#include "GStaticSynapses.h"

const Type StaticSynapse::type = Static;

StaticSynapse::StaticSynapse(ID id, real weight, real delay, real tau_syn)
	: SynapseBase(id), _weight(weight), _delay(delay), _tau_syn(tau_syn)
{
	this->monitored = false;
}

StaticSynapse::StaticSynapse(const StaticSynapse &synapse, ID id) : SynapseBase(id)
{
	this->_weight = synapse._weight;
	this->_delay = synapse._delay;
	this->_tau_syn = synapse._tau_syn;
	this->monitored = false;
}

StaticSynapse::~StaticSynapse()
{
	delay_queue.clear();
}

void StaticSynapse::setDst(NeuronBase *p)
{
	pDest = p;
}

int StaticSynapse::recv()
{
	delay_queue.push_back(delay_steps);

	return 0;
}

Type StaticSynapse::getType()
{
	return type;
}

int StaticSynapse::reset(SimInfo &info)
{
	real dt = info.dt;
	delay_steps = static_cast<int>(_delay/dt);
        real _C1 = expf(-(_delay-dt*delay_steps)/_tau_syn);
	this->_weight = this->_weight * _C1;

	return 0;
}

int StaticSynapse::update(SimInfo &info)
{
	list<int>::iterator iter;

	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
		pDest->recv(_weight);
		delay_queue.pop_front();
	}

	for (iter = delay_queue.begin(); iter != delay_queue.end(); iter++) {
		*iter = *iter - 1;
	}

	return 0;
}

void StaticSynapse::monitor(SimInfo &info)
{
}

size_t StaticSynapse::getSize()
{
	return sizeof(GStaticSynapses);
}

int StaticSynapse::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID().getId();
	(*p)["weight"] = _weight;

	return 0;
}

int StaticSynapse::hardCopy(void * data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GStaticSynapses *p = (GStaticSynapses *) data;
	id2idx[getID()] = idx + base;
	setIdx(idx+base);
	idx2id[idx+base] = getID();
	p->p_weight[idx] = _weight;

	return 1;
}

