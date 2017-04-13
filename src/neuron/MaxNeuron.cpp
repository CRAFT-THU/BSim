/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "MaxNeuron.h"
#include "GMaxNeurons.h"

const Type MaxNeuron::type = Max;

MaxNeuron::MaxNeuron(ID id, int N) : NeuronBase(id)
{
	this->fired = false;
	this->monitored = false;
	assert(N < 32 && N >= 0);
	this->_N = N;
	this->_count = 0;
	this->_idxs = 0;
	this->_record = (int*)(malloc(sizeof(int)*N));
	assert(this->_record != NULL);
	memset(this->_record, 0, sizeof(int)*N);
}

MaxNeuron::MaxNeuron(const MaxNeuron &neuron, ID id) : NeuronBase(id)
{
	this->fired = false;
	this->monitored = false;
	this->_N = neuron._N;
	assert(_N < 32 && _N >= 0);
	this->_count = 0;
	this->_idxs = 0;
	this->_record = (int*)(malloc(sizeof(int)*this->_N));
	assert(this->_record != NULL);
	memset(this->_record, 0, sizeof(int)*this->_N);
}

MaxNeuron::MaxNeuron(const MaxNeuron &neuron) : NeuronBase(neuron)
{
	this->fired = false;
	this->monitored = false;
	this->_N = neuron._N;
	assert(_N < 32 && _N >= 0);
	this->_count = 0;
	this->_idxs = 0;
	this->_record = (int*)(malloc(sizeof(int)*this->_N));
	assert(this->_record != NULL);
	memset(this->_record, 0, sizeof(int)*this->_N);
}

MaxNeuron::~MaxNeuron()
{
	if (this->_record != NULL) {
		free(this->_record);
	}
}

int MaxNeuron::reset(SimInfo &info)
{
	return 0;
}

int MaxNeuron::recv(real I)
{
	this->_idxs += static_cast<int>(I);

	return 0;
}

Type MaxNeuron::getType()
{
	return type;
}

int MaxNeuron::update(SimInfo &info)
{
	bool fired = false;
	int test = 1;
	for (int i=0; i<_N; i++) {
		if (_idxs & test) {
			this->_record[i]++;
			if (this->_record[i] > _count) {
				fired = true;
			}
		}
		test = test << 1;
	}

	_idxs = 0;

	if (fired) {
		_count++;
		fire();
		info.fired.push_back(getID());
		return 1;
	} else {
		return -1;
	}
}

void MaxNeuron::monitor(SimInfo &info) 
{
	return;
}

size_t MaxNeuron::getSize()
{
	return sizeof(GMaxNeurons);
}

int MaxNeuron::getData(void *data)
{
	return 0;
}

int MaxNeuron::hardCopy(void *data, int idx, int base, map<ID, int> &id2idx, map<int, ID> &idx2id)
{
	GMaxNeurons *p = (GMaxNeurons*)data;
	id2idx[getID()] = idx + base;
	setIdx(idx+base);
	idx2id[idx+base] = getID();

	p->p_N[idx] = _N;
	//p->p_count[idx] = _count;
	if (_N > p->max_N) {
		p->max_N = _N;
	}
	
	return 1;
}
