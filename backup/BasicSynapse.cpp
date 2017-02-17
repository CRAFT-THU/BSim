/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include <math.h>

#include "../third_party/json/json.h"
#include "BasicSynapse.h"
#include "GBasicSynapses.h"

const Type BasicSynapse::type = Basic;

BasicSynapse::BasicSynapse(ID id, real weight, real delay, real tau_syn)
{
	this->weight = weight;
	this->delay = delay;
	this->id = id;
	//this->bias = bias;
	this->monitored = false;
	file = NULL;
}

BasicSynapse::~BasicSynapse()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

	delay_queue.clear();
}

void BasicSynapse::setDst(NeuronBase *p) {
	pDest = p;
}

int BasicSynapse::reset(SimInfo &info) {
	delay_steps = static_cast<int>(delay/(info.dt));

	return 0;
}

int BasicSynapse::update(SimInfo &info)
{
	
	list<int>::iterator iter;

	while (!delay_queue.empty() && (delay_queue.front() <= 0)) {
		pDest->recv(weight);
		delay_queue.pop_front();
		info.fired.push_back(id);
	}

	for (iter = delay_queue.begin(); iter != delay_queue.end(); iter++) {
		*iter = *iter - 1;
	}

	return 0;
}

int BasicSynapse::recv()
{
	delay_queue.push_back(delay_steps);

	return 0;
}

Type BasicSynapse::getType()
{
	return type;
}

void BasicSynapse::monitorOn() 
{
	monitored = true;
}

void BasicSynapse::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Synapse_%d.log", id.id);
			file = fopen(filename, "w+");
			if (file == NULL) {
				printf("Open file %s failed\n", filename);
				return;
			}
			fprintf(file, "W: %f, D: %f\n", weight, delay);
		}
	}

	return;
}

size_t BasicSynapse::getSize()
{
	return sizeof(GBasicSynapses);
}

int BasicSynapse::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;
	(*p)["weight"] = weight;
	(*p)["delay"] = delay;

	return 0;
}

int BasicSynapse::hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GBasicSynapses *p = (GBasicSynapses *)data;
	id2idx[id] = idx + base;
	idx2id[idx+base] = id;

	p->p_weight[idx] = weight;
	p->p_delay_steps[idx] = delay_steps;

	return 1;
}
