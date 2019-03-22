/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "ConstantNeuron.h"
#include "GConstantNeurons.h"

const Type ConstantNeuron::type = Constant;

ConstantNeuron::ConstantNeuron(real fire_rate, real start_time, real end_time) : NeuronBase()
{
	this->fire_rate = fire_rate;
	this->start_cycle = 0;
	this->end_cycle = 0;

	this->start_time = start_time;
	this->end_time = end_time;

	//this->tau_syn_E = tau_syn_E;
	//this->tau_syn_I = tau_syn_I;
	file = NULL;
	fired = false;
	monitored = false;

}

ConstantNeuron::ConstantNeuron(const ConstantNeuron &templ) : NeuronBase()
{
	file = NULL;
	fired = false;
	this->fire_rate = templ.fire_rate;
	this->start_time = templ.start_time;
	this->end_time = templ.end_time;
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
	this->start_cycle = static_cast<int>(start_time/info.dt);
	this->end_cycle = static_cast<int>(end_time/info.dt);
	
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
	if (info.currCycle < end_time && (info.currCycle - start_time) * fire_rate > fire_count) {
		fired = true;
		fire_count++;
		fire();
	}
	info.fired.push_back(getID());
	return 0;
}

void ConstantNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "ConstantNeuron_%d.log", getID());
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

void ConstantNeuron::setRate(real rate) {
	this->fire_rate = rate;
}

void ConstantNeuron::setStart(real time) {
	this->start_time = time;
}

void ConstantNeuron::setEnd(real time) {
	this->end_time = time;
}

size_t ConstantNeuron::getSize() {
	return sizeof(GConstantNeurons);
}

int ConstantNeuron::getData(void *data)
{
	return 0;
}

int ConstantNeuron::hardCopy(void *data, int idx, int base)
{
	GConstantNeurons *p = (GConstantNeurons*) data;
	setID(idx+base);

	p->p_fire_rate[idx] = fire_rate;
	p->p_start_cycle[idx] = start_cycle;
	p->p_end_cycle[idx] = end_cycle;
	p->p_fire_count[idx] = fire_count;


	return 1;
}

