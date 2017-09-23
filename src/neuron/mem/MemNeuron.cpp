/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "MemNeuron.h"
#include "GMemNeurons.h"

const Type MemNeuron::type = Mem;

MemNeuron::MemNeuron(real fire_rate/*, real tau_syn_E, real tau_syn_I*/) : NeuronBase()
{
	this->fire_rate = fire_rate;
	//this->tau_syn_E = tau_syn_E;
	//this->tau_syn_I = tau_syn_I;
	file = NULL;
	fired = false;
	monitored = false;

}

MemNeuron::MemNeuron(const MemNeuron &templ) : NeuronBase()
{
	file = NULL;
	fired = false;
	this->fire_rate = templ.fire_rate;
	monitored = templ.monitored;
}

MemNeuron::~MemNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int MemNeuron::reset(SimInfo &info)
{
	fired = false;
	this->fire_count = 0;
	
	return 0;
}

int MemNeuron::recv(real I) {
	return 0;
}

Type MemNeuron::getType()
{
	return type;
}

int MemNeuron::update(SimInfo &info)
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

void MemNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "MemNeuron_%d.log", getID());
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

void MemNeuron::setRate(real rate) {
	this->fire_rate = rate;
}

size_t MemNeuron::getSize() {
	return sizeof(GMemNeurons);
}

int MemNeuron::getData(void *data)
{
	return 0;
}

int MemNeuron::hardCopy(void *data, int idx, int base)
{
	GMemNeurons *p = (GMemNeurons*) data;
	setID(idx+base);

	p->p_fire_rate[idx] = fire_rate;
	p->p_fire_count[idx] = fire_count;


	return 1;
}

