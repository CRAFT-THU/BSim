/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "ConstantNeuron.h"
#include "GConstantNeurons.h"

const Type ConstantNeuron::type = Constant;

ConstantNeuron::ConstantNeuron(ID id, real fire_rate, real tau_syn_E, real tau_syn_I) : NeuronBase(id)
{
	this->fire_rate = fire_rate;
	this->tau_syn_E = tau_syn_E;
	this->tau_syn_I = tau_syn_I;
	file = NULL;
	fired = false;
	monitored = false;

}

ConstantNeuron::ConstantNeuron(const ConstantNeuron &templ, ID id) : NeuronBase(id)
{
	file = NULL;
	fired = false;
	this->fire_rate = templ.fire_rate;
	monitored = templ.monitored;
}

ConstantNeuron::~ConstantNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int ConstantNeuron::reset(SimInfo &info)
{
	fired = false;
	this->fire_count = 0;
	
	return 0;
}

int ConstantNeuron::recv(real I) {
	return 0;
}

Type ConstantNeuron::getType()
{
	return type;
}

int ConstantNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle * fire_rate > fire_count) {
		fired = true;
		fire_count++;
		fire();
		info.fired.push_back(getID());
	}
	return 0;
}

void ConstantNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "ConstantNeuron_%s.log", getID().getInfo().c_str());
			file = fopen(filename, "w+");
			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}
		} else {
			if (fired) {
				fprintf(file, "%d:%d\n", info.currCycle, fire_count);
			}
		}
	}
	return;
}

size_t ConstantNeuron::getSize() {
	return sizeof(GConstantNeurons);
}

int ConstantNeuron::getData(void *data)
{
	return 0;
}

int ConstantNeuron::hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GConstantNeurons *p = (GConstantNeurons*) data;
	id2idx[getID()] = idx + base;
	idx2id[idx+base] = getID();

	p->p_fire_rate[idx] = fire_rate;
	p->p_fire_count[idx] = fire_count;


	return 1;
}

