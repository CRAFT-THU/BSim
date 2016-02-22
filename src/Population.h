/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef POPULATION_H
#define POPULATION_H

#include <vector>

#include "utils/json/json.h"
#include "PopulationBase.h"

using std::vector;

template<class Neuron>
class Population : public PopulationBase {
public:
	Population(ID _id, unsigned int N);
	~Population();

	ID getID();
	int getNum();
	size_t getSize();

	int reset(SimInfo &info);
	int update(SimInfo &info);
	void monitor(SimInfo &info);

	int getData(void *data);
	unsigned int hardCopy(void *data, unsigned int idx);

	int addNeuron(Neuron templ);
	NeuronBase* findNeuron(ID id);
	NeuronBase* getNeuron(int idx);
protected:
	ID id;
	unsigned int N;
	vector<Neuron> neurons;
};

#endif /* POPULATION_H */

template<class Neuron>
Population<Neuron>::Population(ID _id, unsigned int N) : id(_id)
{
	neurons.reserve(N);
	this->N = N;
}

template<class Neuron>
Population<Neuron>::~Population() 
{
	neurons.clear();
	N = 0;
}

template<class Neuron>
int Population<Neuron>::getNum()
{
	return N;
}

template<class Neuron>
NeuronBase* Population<Neuron>::getNeuron(int idx)
{
	return &neurons[idx];
}

template<class Neuron>
NeuronBase* Population<Neuron>::findNeuron(ID id)
{
	NeuronBase *ret = NULL;
	for (unsigned int i=0; i< neurons.size(); i++) {
		if (neurons[i].getID() == id) {
			ret = &(neurons[i]);
			break;
		}
	}

	return ret;
}

template<class Neuron>
void Population<Neuron>::monitor(SimInfo &info)
{
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		iter->monitor(info);
	}
}

template<class Neuron>
int Population<Neuron>::update(SimInfo &info)
{
	int ret = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		ret += iter->update(info);
	}
	return ret;
}

template<class Neuron>
int Population<Neuron>::reset(SimInfo &info)
{
	int ret = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		ret += iter->reset(info);
	}
	return ret;
}

template<class Neuron>
size_t Population<Neuron>::getSize()
{
	if (neurons.size() > 0) {
		return N*neurons[0].getSize();
	} else {
		return 0;
	}
}

template<class Neuron>
int Population<Neuron>::addNeuron(Neuron templ)
{
	neurons.push_back(templ);
	return neurons.size() - 1;
}

template<class Neuron>
unsigned int Population<Neuron>::hardCopy(void *data, unsigned int idx)
{
	size_t copiedIdxs = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		size_t copied = iter->hardCopy(data, idx+copiedIdxs);
		copiedIdxs += copied;
	}
	return copiedIdxs;
}

template<class Neuron>
ID Population<Neuron>::getID() {
	return id;
}

template<class Neuron>
int Population<Neuron>::getData(void *data) {
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = id.id;
	(*p)["size"] = N;
	(*p)["neuron_type"] = 0;
	Json::Value parameters(Json::arrayValue);
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		Json::Value parameter;
		iter->getData(&parameter);
		parameters.append(parameter);
	}
	(*p)["parameters"] = parameters;

	return 0;
}
