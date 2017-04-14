/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include <math.h>

#include "../third_party/json/json.h"
#include "ExpSynapse.h"
#include "GExpSynapses.h"

const Type ExpSynapse::type = Exp;

using std::map;

ExpSynapse::ExpSynapse(real weight, real delay = 0, real tau_syn = 0) : SynapseBase(node, weight)
{
	this->delay = delay;
	this->tau_syn = tau_syn;
	this->monitored = false;
	file = NULL;
}

ExpSynapse::~ExpSynapse()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

	delay_queue.clear();
}

int ExpSynapse::reset(SimInfo &info) {
	I_syn = 0;
	init(info.dt);

	return 0;
}

int ExpSynapse::init(real dt) {
	//_dt = dt;
	_delay_steps = static_cast<int>(delay/dt);
	if (tau_syn > 0) {
		C1 = expf(-dt/tau_syn);
		_C1 = expf(-(delay-dt*_delay_steps)/tau_syn);
		active_steps = static_cast<int>(DECAY_MULTIPLE_TAU * tau_syn/dt + 0.5);
		// To be identified with brian2
		//this->weight = this->weight * expf(-dt/tau_syn/2.0f);
	} else {
		C1 = 0;
		_C1 = 1.0;
		active_steps = 1;
		printf("Tau of synapse %d is ZERO, Please make sure!\n", getID());
		return -1;
	}


	return 0;
}

int ExpSynapse::update(SimInfo &info)
{
	I_syn = C1 * I_syn;
	
	list<int>::iterator iter;

	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
		I_syn += _weight/_C1;
		//pDest->recv(I_syn);
		delay_queue.pop_front();
		//info.fired.push_back(getID());
		step_to_zero = active_steps;
	}

	if (step_to_zero > 0) {
		pDest->recv(I_syn);
		step_to_zero--;
	} else {
		I_syn = 0;
	}

	for (iter = delay_queue.begin(); iter != delay_queue.end(); iter++) {
		*iter = *iter - 1;
	}

	//info.input.push_back(I_syn);

	return 0;
}

int ExpSynapse::recv()
{
	delay_queue.push_back(delay_steps);

	return 0;
}

Type ExpSynapse::getType()
{
	return type;
}

void ExpSynapse::monitorOn() 
{
	monitored = true;
}

void ExpSynapse::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Synapse_%d.log", getID().getId());
			file = fopen(filename, "w+");
			if (file == NULL) {
				printf("Open file %s failed\n", filename);
				return;
			}
			fprintf(file, "W: %f, D: %f, T:%f\n", weight, delay, tau_syn);
			fprintf(file, "C1: %f, ", C1);
		}
		fprintf(file, "Cycle %d: %f\n", info.currCycle, this->I_syn); 
	}

	return;
}

size_t ExpSynapse::getSize()
{
	return sizeof(GExpSynapses);
}

int ExpSynapse::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["weight"] = _weight;
	(*p)["delay"] = delay;

	return 0;
}

int ExpSynapse::hardCopy(void *data, int idx, int base)
{
	GExpSynapses *p = (GExpSynapses *)data;
	setID(idx+base);
	p->p_weight[idx] = _weight;
	p->p_delay_steps[idx] = _delay_steps;
	p->p_active_steps[idx] = active_steps;
	p->p_C1[idx] = C1;
	p->p__C1[idx] = _C1;
	p->p_I_syn[idx] = I_syn;
	p->p_dst[idx] = this->getDst()->getID();
	return 1;
}
