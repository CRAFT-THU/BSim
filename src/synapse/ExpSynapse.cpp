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

ExpSynapse::ExpSynapse(ID id, real weight, real delay = 0, real tau_syn = 0) : SynapseBase(id)
{
	this->weight = weight;
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

void ExpSynapse::setDst(NeuronBase *p) {
	pDest = p;
}

int ExpSynapse::reset(SimInfo &info) {
	I_syn = 0;
	init(info.dt);

	return 0;
}

int ExpSynapse::init(real dt) {
	//_dt = dt;
	delay_steps = static_cast<int>(delay/dt);
	if (tau_syn > 0) {
		C1 = expf(-dt/tau_syn);
		_C1 = expf(-(delay-dt*delay_steps)/tau_syn);
	} else {
		printf("Wrong tau!\n");
		return -1;
	}

	return 0;
}

int ExpSynapse::update(SimInfo &info)
{
	I_syn = C1 * I_syn;
	
	list<int>::iterator iter;

	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
		I_syn += weight/_C1;
		pDest->recv(I_syn);
		delay_queue.pop_front();
		info.fired.push_back(getID());
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
	(*p)["weight"] = weight;
	(*p)["delay"] = delay;

	return 0;
}

int ExpSynapse::hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GExpSynapses *p = (GExpSynapses *)data;
	id2idx[this->getID()] = idx + base;
	idx2id[idx+base] = this->getID();
	//p->pID[idx] = id;
	//p->pType[idx] = type;
	p->p_weight[idx] = weight;
	p->p_delay_steps[idx] = delay_steps;
	//p->p_delay[idx] = delay;
	p->p_C1[idx] = C1;
	p->p__C1[idx] = _C1;
	//p->p_tau_syn[idx] = tau_syn;
	p->p_I_syn[idx] = I_syn;
	//p->p__dt[idx] = _dt;
	return 1;
}
