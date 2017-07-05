/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "../third_party/json/json.h"
#include "TJNeuron.h"
#include "GTJNeurons.h"

using std::vector;
using std::map;

const Type TJNeuron::type = TJ;

TJNeuron::TJNeuron(real v_init, real v_reset, real cm, real tau_refrac, real v_thresh, real i_offset)
	: NeuronBase(), v_init(v_init), v_reset(v_reset), cm(cm), tau_refrac(tau_refrac), v_thresh(v_thresh), i_offset(i_offset)
{
	this->i_syn = 0;
	this->file = NULL;
	this->monitored = false;
}

TJNeuron::TJNeuron(const TJNeuron &neuron) : NeuronBase()
{
	this->v_init = neuron.v_init;
	this->v_reset = neuron.v_reset;
	this->cm = neuron.cm;
	this->tau_refrac = neuron.tau_refrac;
	this->v_thresh = neuron.v_thresh;
	this->i_offset = neuron.i_offset;
	this->i_syn = 0;
	this->file = NULL;
}

TJNeuron::~TJNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int TJNeuron::init(real dt)
{
	//_dt = dt;
	i_tmp = i_offset;

	refrac_time = static_cast<int>(tau_refrac/dt);
	refrac_step = refrac_time;
	
	return 0;
}

int TJNeuron::update(SimInfo &info)
{
	fired = false;

	if (refrac_step > 0) {
		--refrac_step;
	} else {
		real I = i_syn + i_tmp;
		vm =  vm + I*info.dt/cm;
		
		if (vm >= v_thresh) {
			fired = true;
			refrac_step = refrac_time - 1;
			vm = v_reset;
		}

	}

	info.input.push_back(vm);
	info.input.push_back(i_syn);
	info.input.push_back(0);
	info.input.push_back(0);
	info.input.push_back(0);

	i_syn = 0;

	info.fired.push_back(fired);
	if (fired) {
		fire();
		return 1;
	} else {
		return -1;
	}
}

int TJNeuron::recv(real I)
{
	i_syn += I;
	return 0;
}

int TJNeuron::reset(SimInfo &info)
{
	vm = v_init;
	refrac_step = -1;
	i_syn = 0;
	return init(info.dt);
}

Type TJNeuron::getType()
{
	return type;
}

real TJNeuron::get_vm()
{
	return vm;
}

void TJNeuron::monitor(SimInfo &info) 
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Neuron_%d.log", getID());
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}

		}
		fprintf(file, "Cycle %d: vm = %f\n", info.currCycle, vm); 
	}
}

size_t TJNeuron::getSize()
{
	return sizeof(GTJNeurons);
}

int TJNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;

	(*p)["id"] = getID();
	(*p)["v_init"] = v_init;
	(*p)["v_reset"] = v_reset;
	(*p)["cm"] = cm;
	(*p)["tau_refrac"] = tau_refrac;
	(*p)["v_thresh"] = v_thresh;
	(*p)["i_offset"] = i_offset;

	return 0;
}

int TJNeuron::hardCopy(void *data, int idx, int base)
{
	GTJNeurons * p = (GTJNeurons *) data;
	setID(idx+base);
	p->p_v_reset[idx] = v_reset;
	p->p_v_thresh[idx] = v_thresh;
	p->p_vm[idx] = vm;
	p->p_i_tmp[idx] = i_tmp;
	p->p_cm[idx] = cm;
	p->p_refrac_step[idx] = refrac_step;
	p->p_refrac_time[idx] = refrac_time;
	
	return 1;
}
