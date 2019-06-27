/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef POPULATION_H
#define POPULATION_H

#include <vector>

#include "../third_party/json/json.h"
#include "PopulationBase.h"

using std::vector;

template<class Neuron>
class Population : public PopulationBase {
public:
	Population(size_t N);
	Population(size_t N, const Neuron &templ);
	Population(int id, size_t N, const Neuron &templ);
	~Population();

	Type getType();

	int getNum();
	virtual size_t getSize();

	// virtual void setNode(int node) final;
	// virtual int reset(SimInfo &info);
	// virtual int update(SimInfo &info);
	// virtual void monitor(SimInfo &info);

	virtual int getData(void *data);
	virtual int hardCopy(void *data, int idx, int base, SimInfo &info);

	int addNeuron(Neuron templ);
	NeuronBase* findNeuron(ID id);
	NeuronBase* getNeuron(int idx);
protected:
	const static Type type;

	size_t N;
	vector<Neuron> neurons;
};

#endif /* POPULATION_H */

template<class Neuron>
const Type Population<Neuron>::type = Neuron::type;

template<class Neuron>
Population<Neuron>::Population(size_t N) : PopulationBase()
{
	neurons.reserve(N);
	this->N = N;
}

template<class Neuron>
Population<Neuron>::Population(size_t N, const Neuron &templ) : PopulationBase()
{
	neurons.resize(N, templ);
	this->N = N;
}

template<class Neuron>
Population<Neuron>::Population(ID id, size_t N, const Neuron &templ) : PopulationBase()
{
	neurons.resize(N, templ);
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
	for (unsigned int i=0; i< neurons.size(); i++) {
		if (neurons[i].getID() == id) {
			return &(neurons[i]);
		}
	}

	return NULL;
}


//template<class Neuron>
//void Population<Neuron>::setNode(int node)
//{
//	Base::setNode(node);
//	typename vector<Neuron>::iterator iter;
//	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
//		setNode(node);
//	}
//}

// template<class Neuron>
// void Population<Neuron>::monitor(SimInfo &info)
// {
// 	if(monitored) {
// 		typename vector<Neuron>::iterator iter;
// 		for (iter = neurons.begin(); iter != neurons.end(); iter++) {
// 			iter->monitorOn();
// 			iter->monitor(info);
// 		}
// 	}
// }

// template<class Neuron>
// int Population<Neuron>::update(SimInfo &info)
// {
// 	int ret = 0;
// 	typename vector<Neuron>::iterator iter;
// 	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
// 		ret += iter->update(info);
// 	}
// 	return ret;
// }
// 
// template<class Neuron>
// int Population<Neuron>::reset(SimInfo &info)
// {
// 	int ret = 0;
// 	typename vector<Neuron>::iterator iter;
// 	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
// 		ret += iter->reset(info);
// 	}
// 	return ret;
// }

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
	if (neurons.size() > N) {
		printf("Population overflow!!");
		N = neurons.size();
	}

	return neurons.size() - 1;
}

template<class Neuron>
int Population<Neuron>::hardCopy(void *data, int idx, int base, SimInfo &info)
{
	size_t copiedIdxs = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		size_t copied = iter->hardCopy(data, idx+copiedIdxs, base, info);
		copiedIdxs += copied;
	}
	return copiedIdxs;
}

template<class Neuron>
Type Population<Neuron>::getType() {
	return type;
}

template<class Neuron>
int Population<Neuron>::getData(void *data) {
	Json::Value *p = (Json::Value *)data;
	(*p)["id"] = getID();
	(*p)["size"] = (unsigned int)N;
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
