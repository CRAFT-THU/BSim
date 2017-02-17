/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>
#include <algorithm>

#include "utils/json/json.h"
#include "MaxNeuron.h"
//#include "GMaxNeurons.h"

using std::vector;
using std::max;

const Type MaxNeuron::type = LIF;

MaxNeuron::MaxNeuron(ID id, int cycle)
{
	this->id = id;
	this->file = NULL;
	this->cycle = cycle;
	this->monitored = false;
}

MaxNeuron::MaxNeuron(const MaxNeuron &neuron, ID id)
{
	this->id = id;
	this->file = NULL;
	this->cycle = neuron.cycle;
	this->monitored = neuron.monitored;
}

MaxNeuron::~MaxNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int MaxNeuron::update(SimInfo &info)
{
	fired = false;

	if (fired_count < max_input) {
		fired = true;
	}

	if (fired) {
		fire();
		info.fired.push_back(this->id);
		return 1;
	} else {
		return -1;
	}
}

int MaxNeuron::recv(real I)
{
	int idx = (int)I;
	input_count[idx] = input_count[idx] + 1;
	max_input = max(max_input, input_count[idx]);
	return 0;
}

int MaxNeuron::reset(SimInfo &info)
{
	_dt = info.dt;
	return 0;
}


Type MaxNeuron::getType()
{
	return type;
}

real MaxNeuron::get_vm()
{
	return 0;
}

void MaxNeuron::monitor(SimInfo &info) 
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Neuron_%d_%d.log", id.groupId, id.id);
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}

		}
		if (fired) {
			fprintf(file, "Cycle %d\n", info.currCycle); 
		}
	}
}

size_t MaxNeuron::getSize()
{
	return 0;
}

int MaxNeuron::getData(void *data)
{
	return 0;
}

int MaxNeuron::hardCopy(void *data, int idx)
{
	return 1;
}

int MaxNeuron::fire()
{
	return 0;
}	

SynapseBase* MaxNeuron::addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest)
{
	return NULL;
}
