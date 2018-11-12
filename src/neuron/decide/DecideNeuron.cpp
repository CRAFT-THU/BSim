/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "DecideNeuron.h"
#include "GDecideNeurons.h"

const Type DecideNeuron::type = Decide;

DecideNeuron::DecideNeuron() : NeuronBase()
{
	this->fire_rate = 0;
	this->tmp_rate = 0;
	this->fire_count = 0;
	//this->tau_syn_E = tau_syn_E;
	//this->tau_syn_I = tau_syn_I;
	file = NULL;
	fired = false;
	monitored = false;

}

DecideNeuron::DecideNeuron(const DecideNeuron &templ) : NeuronBase()
{
	file = NULL;
	fired = false;
	this->fire_rate = templ.fire_rate;
	this->tmp_rate = templ.tmp_rate;
	this->fire_count = templ.fire_count;
	monitored = templ.monitored;
}

DecideNeuron::~DecideNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int DecideNeuron::reset(SimInfo &info)
{
	fired = false;
	this->fire_count = 0;
	this->fire_rate = 0;
	this->tmp_rate = 0;
	
	return 0;
}

int DecideNeuron::recv(real I) {
	return 0;
}

Type DecideNeuron::getType()
{
	return type;
}

int DecideNeuron::update(SimInfo &info)
{
	fired = false;
	if (info.currCycle * fire_rate > fire_count) {
		fired = true;
		fire_count++;
		fire();
	}
	info.fired.push_back(getID());
	return 0;
}

void DecideNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "DecideNeuron_%d.log", getID());
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

size_t DecideNeuron::getSize() {
	return sizeof(GDecideNeurons);
}

int DecideNeuron::getData(void *data)
{
	return 0;
}

int DecideNeuron::hardCopy(void *data, int idx, int base)
{
	GDecideNeurons *p = (GDecideNeurons*) data;
	setID(idx+base);

	p->p_tmp_rate[idx] = tmp_rate;
	p->p_fire_rate[idx] = fire_rate;
	p->p_fire_count[idx] = fire_count;


	return 1;
}

